#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <complex>
#include <chrono>
//#include <string.h>
#include <exception>
#include "lua.hpp"

#include "Naming.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "interfaces/IEventListener.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractInitilizer.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractNoise.hpp"
#include "EntryPointBase/AbstractOutput.hpp"
#include "lua/lua_static.hpp"
#include "lua/types/lua_complex.hpp"
#include "lua/types/matrix/lua_matrix_common.hpp"
#include "lua/types/matrix/lua_matrix_real.hpp"
#include "lua/lua_random.hpp"
#include "lua/lua_math.hpp"
#include "types/lua_basetypes.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ModuleLoader.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/LuaParser.hpp"
#include "LoggingSystem.hpp"

LuaParser::LuaParser(const std::string& filename, std::vector<char*>& argv, ModuleLoader* loader)
{
    _loader = loader;
    _filename = new std::string(filename);
    _L = luaL_newstate();
    _rL = _L;
    // if some argv:
    // luaL_openlibs(_L);

    // set in registry LuaParser and ModuleLoader
    lua_pushstring(_L, Naming::Lua_reg_instance_LuaParser);
    lua_pushlightuserdata(_L, (void *)this);
    lua_settable(_L, LUA_REGISTRYINDEX);

    lua_pushstring(_L, Naming::Lua_reg_instance_ModuleLoader);
    lua_pushlightuserdata(_L, (void *)_loader);
    lua_settable(_L, LUA_REGISTRYINDEX);

    lua_pushstring(_L, Naming::Lua_reg_references);
    lua_newtable(_L);
    lua_settable(_L, LUA_REGISTRYINDEX);

    //lua_pushstring(_L, Naming::Lua_reg_functions);
    //lua_newtable(_L);
    //lua_settable(_L, LUA_REGISTRYINDEX);

    lua_pushstring(_L, Naming::Lua_reg_argv);
    lua_createtable(_L, argv.size(), 0);
    for(unsigned int i = 0; i < argv.size(); i++) {
        lua_pushstring(_L, argv[i]);
        lua_rawseti (_L, -2, i);
    }
    lua_settable(_L, LUA_REGISTRYINDEX);

    // create table to store all system functions
    // and keywords that cannot be overridden
    lua_pushstring(_L, Naming::Lua_reg_systemNames);
    lua_newtable(_L);
    lua_settable(_L, LUA_REGISTRYINDEX);

    // create meta tables for global scope and userdata
    ParameterTypeSystem::connect(this);
    for(auto it = ParameterTypeSystem::_instance._typeList->begin(); it != ParameterTypeSystem::_instance._typeList->end(); it++)
    {
        if((*it)->getID() > 4)
            notifyLoad(*it);
    }

    init();

    EntryPointSystem::connect(this);
    // set all already available EntryPoints
    for(auto it = EntryPointSystem::begin(); it != EntryPointSystem::end(); it++)
    {
        pushEntryPoint(*it);
    }
}

void LuaParser::init()
{
    //EntryPointSystem::_instance._luaparser = this;
    //ParameterTypeSystem::_instance._luaparser = this;

    lua_pushglobaltable(_L);
    // meta table for global with __index and __newindex
    lua_newtable(_L);

    // set system names table as __index, so they cannot be set by script
    lua_pushstring(_L, "__index");
    lua_pushstring(_L, Naming::Lua_reg_systemNames);
    lua_gettable(_L, LUA_REGISTRYINDEX);

    // add all system function
    lua_pushstring(_L, Naming::Lua_global_log);
    lua_pushcfunction (_L, lua_global_log);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_print);
    lua_pushcfunction (_L, lua_global_print);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_write);
    lua_pushcfunction (_L, lua_global_write);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_writeline);
    lua_pushcfunction (_L, lua_global_writeline);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_error);
    lua_pushcfunction (_L, lua_global_error);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_warning);
    lua_pushcfunction (_L, lua_global_warning);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_exit);
    lua_pushcfunction (_L, lua_global_exit);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_simulation);
    lua_pushcfunction (_L, lua_global_simulation);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_complex);
    push_complex_library(_L);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_random);
    push_random_library(_L);
    lua_rawset(_L, -3);

    lua_pushstring(_L, Naming::Lua_global_math);
    push_math_library(_L);
    lua_rawset(_L, -3);

    ParameterTypeSystem::registerParameter(Naming::Type_Complex, {luat_complex_init, luat_UserData_push, luat_complex_delete});
    lua_pushstring(_L, "J");
    std::complex<double>** c = (std::complex<double>**)lua_newuserdata(_L, sizeof(std::complex<double>*));
    *c = new std::complex<double>(0.0, 1.0);
    std::string metaComplex(std::string("meta:") + std::string(Naming::Type_Complex));
    luaL_setmetatable(_L, metaComplex.c_str());
    lua_rawset(_L, -3);

    ParameterTypeSystem::registerMatrix(ParameterTypeSystem::pid_real, {luat_matrix_real_init, luat_UserData_push, luat_matrix_real_delete});
    lua_pushstring(_L, Naming::Lua_global_matrix);
    push_matrix_library(_L);
    lua_rawset(_L, -3);

    // add argv table, which has all elements as
    // meta table with __index, so elements cannot be changed
    lua_pushstring(_L, Naming::Lua_global_argv);
    lua_newtable(_L);  // value for argv
    lua_newtable(_L);  // meta table for argv

    // set __index in metatable
    lua_pushstring(_L, "__index");
    lua_pushstring(_L, Naming::Lua_reg_argv);
    lua_gettable(_L, LUA_REGISTRYINDEX);
    lua_rawset(_L, -3);

    // set __newindex in metatable
    lua_pushstring(_L, "__newindex");
    lua_pushcfunction (_L, lua_ignore_newindex);
    lua_rawset(_L, -3);

    // set __len in metatable
    lua_pushstring(_L, "__len");
    lua_pushcfunction (_L, lua_argv_len);
    lua_rawset(_L, -3);

    lua_setmetatable(_L, -2);  // set meta table for argv
    lua_rawset(_L, -3);  // set argv
    lua_rawset(_L, -3);  // set table as __index in global metatable

    // set __newindex in global metatable
    lua_pushstring(_L, "__newindex");
    lua_pushcfunction (_L, lua_global_scope_newindex);
    lua_rawset(_L, -3);

    lua_setmetatable(_L, -2);  // set meta table for global table

    ParameterTypeSystem::registerParameter(Naming::Type_Simulation, {nullptr, nullptr, luat_Simulation_delete}, false);

    lua_settop(_L, 0);
}

void LuaParser::pushLuaValue(T_Parameter param)
{
    if(!ParameterTypeSystem::pushValue(_L, param))
    {
        lua_pushnil(_L);
    }
}

void LuaParser::pushLuaValue(int type, void* value)
{
    if(!ParameterTypeSystem::pushValue(_L, {type, value}))
    {
        lua_pushnil(_L);
    }
}

void LuaParser::pushLuaValue(const std::string& type, void* value)
{
    if(!ParameterTypeSystem::pushValue(_L, {ParameterTypeSystem::getParameterID(type), value}))
    {
        lua_pushnil(_L);
    }
}

bool LuaParser::processFile()
{
    int status, result;
    status = luaL_loadfile(_L, _filename->c_str());
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        std::string s("Couldn't load file: ");
        s += lua_tostring(_L, -1);
        LoggingSystem::Error(s);
        return false;
    }

    result = lua_pcall(_L, 0, LUA_MULTRET, 0);

    if (result) {
        std::string s("Failed to run script: ");
        s += lua_tostring(_L, -1);
        LoggingSystem::Error(s);
        return false;
    }

    lua_settop(_L, 0);
    return true;
}

LuaParser::~LuaParser()
{
    /*
    lua_pushstring(_L, "simulation:com:references");
    lua_pushnil(_L);
    lua_settable(_L, LUA_REGISTRYINDEX);
    lua_gc(_L, LUA_GCCOLLECT);
    */
    if(_L)
        lua_close(_L);
    EntryPointSystem::disconnect();
    ParameterTypeSystem::disconnect();

//    for ( auto it = _params->begin(); it != _params->end(); ++it )
//        delete (double*)(it->second);
    delete _filename;
    //delete _params;
    //delete _paramTypes;
    //delete _features;
}

/*
 * implemented interface IEntryPointEventHandler
 */
void LuaParser::notifyLoad(EntryPoint *ep)
{
    if(ep)
    {
        pushEntryPoint(ep);
    }
}

void LuaParser::notifyUnload(EntryPoint *ep)
{

}

void LuaParser::notifyLoad(const std::string& name, EntryPoint *ep, Registry const * const reg)
{
    if(reg)
    {
        pushRegistry(name, reg);
    }
}

void LuaParser::notifyUnload(const std::string& name, EntryPoint *ep, Registry * const reg)
{
    reg->truncate();
    // delete all instances??
    /*
    lua_pushstring(_L, "simulation:system_names");
    lua_gettable(_L, LUA_REGISTRYINDEX);
    lua_pushstring(_L, ep->getName().c_str());
    lua_rawget(_L, -2);
    luaL_getmetafield(_L, -1, "__loaded");
    lua_pushlstring(_L, name.c_str(), name.size());
    lua_pushnil(_L);
    lua_rawset(_L, -3);
    lua_pop(_L, 3);
    _loader->Unload(name);
    */
}

void LuaParser::chainLoad(ParameterType const * const p, const int& id, const std::string& metaName) const
{
    int base = p->getBase();
    int top = lua_gettop(_L);
    if(base)
    {
        ParameterType const * const b = ParameterTypeSystem::getParameter(base);
        luaL_getmetatable (_L, metaName.c_str());
        chainLoad(b, id, metaName);
        lua_settop(_L, top);
    }
    luaL_getmetatable (_L, metaName.c_str());
    p->initialize(_L, id);
    lua_settop(_L, top);
}

void LuaParser::notifyLoad(ParameterType const * const p)
{
    int top = lua_gettop(_L);
    std::string metaName("meta:");
    metaName += p->getName();
    luaL_newmetatable (_L, metaName.c_str());
    chainLoad(p, p->getID(), metaName);
    luaL_getmetatable (_L, metaName.c_str());
    lua_pushcfunction(_L, lua_UserData_gc);
    lua_setfield(_L, -2, "__gc");
    lua_pushlstring(_L, p->getName().c_str(), p->getName().size());
    lua_setfield(_L, -2, "__name");
    lua_pushinteger(_L, p->getID());
    lua_setfield(_L, -2, "__type");
    lua_settop(_L, top);
}

void LuaParser::notifyUnload(ParameterType const * const p)
{

}

lua_State* LuaParser::getLuaState()
{
    return _rL;
}
