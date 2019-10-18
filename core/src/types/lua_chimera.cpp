#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "types/lua_basetypes.hpp"
#include "types/lua_static.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "types/lua_chimera.hpp"

void chimera::ChimeraSystem::init()
{
    _L = luaL_newstate();

    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
    lua_pushlightuserdata(_L, (void *)this);
    lua_settable(_L, LUA_REGISTRYINDEX);

    // create table to store all system functions
    // and keywords that cannot be overridden
    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_newtable(_L);
    lua_settable(_L, LUA_REGISTRYINDEX);

    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_newtable(_L);
    lua_settable(_L, LUA_REGISTRYINDEX);

    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_newtable(_L);
    lua_newtable(_L);
    lua_pushliteral(_L, "__mode");
    lua_pushliteral(_L, "v");
    lua_rawset(_L, -3);
    lua_setmetatable(_L, -2);
    lua_settable(_L, LUA_REGISTRYINDEX);

    lua_pushglobaltable(_L);
    // meta table for global with __index and __newindex
    lua_newtable(_L);

    // set system names table as __index, so they cannot be set by script
    lua_pushstring(_L, "__index");
    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_gettable(_L, LUA_REGISTRYINDEX);
    lua_rawset(_L, -3);  // set table as __index in global metatable

    // set __newindex in global metatable
    lua_pushstring(_L, "__newindex");
    lua_pushcfunction (_L, lua_global_scope_newindex);
    lua_rawset(_L, -3);

    lua_setmetatable(_L, -2);  // set meta table for global table
    lua_pop(_L, 1);

    for(auto it = _typeSys->_typeList->begin(); it != _typeSys->_typeList->end(); it++)
    {
        if((*it)->getID() > chimera::systemtypes::PID_LIBRARAY)
            pushType(*it); // create Meta for Parameter
    }
}

void chimera::ChimeraSystem::pushLuaValue(lua_State* L, chimera::ParameterValue& param) const
{
    _typeSys->pushValue(L, param.getType(), param.getValue());
}

void chimera::ChimeraSystem::pushLuaValue(lua_State* L, size_t type, void* value) const
{
    _typeSys->pushValue(L, type, value);
}

void chimera::ChimeraSystem::pushLuaValue(lua_State* L, const std::string& type, void* value) const
{
    _typeSys->pushValue(L, _typeSys->getParameterID(type), value);
}

void chimera::ChimeraSystem::pushType(ParameterType const * const p)
{
    // Create new meta table for new type
    if(p != nullptr)
    {
        int type = p->getID();
        std::string s(p->getName());
        lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_gettable(_L, LUA_REGISTRYINDEX);
        // new meta table
        lua_newtable(_L);
        // pops table above and returns / pushes a single table onto stack
        // thus chainLoad will leave stack count unchanged
        // executes all init functions of type and base types
        chainLoad(p, p->getID());
        lua_pushcfunction(_L, chimera::lua_UserData_gc);
        lua_setfield(_L, -2, "__gc");
        lua_pushstring(_L, p->getName().c_str());
        lua_setfield(_L, -2, "__name");
        lua_pushinteger(_L, p->getID());
        lua_setfield(_L, -2, "__type");
        lua_rawseti(_L, -2, p->getID());
        lua_pop(_L, 1);
    }
}

void chimera::ChimeraSystem::popType(const ParameterType& type)
{
    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_gettable(_L, LUA_REGISTRYINDEX);
    lua_pushnil(_L);
    lua_rawseti(_L, -2, type.getID());
    lua_pop(_L, 1);
}

lua_State* chimera::ChimeraSystem::getLuaState()
{
    return _L;
}

void chimera::ChimeraSystem::chainLoad(ParameterType const * const p, const int& id) const
{
    int base = p->getBase();
    if(base)
    {
        ParameterType const * const b = _typeSys->getParameter(base);
        chainLoad(b, id);
    }
    p->initialize(_L, id);
    // if a initialize call leaves any object but a table
    // replace it with table again, so it can be used as a
    // meta table
    if(lua_type(_L, -1) != LUA_TTABLE)
    {
        lua_pop(_L, 1);
        lua_newtable(_L);
    }
}

void chimera::ChimeraSystem::pushLibrary(const std::string& name, lua_CFunction createMetaTable)
{
    lua_pushcfunction(_L, createMetaTable);
    lua_call(_L, 0, 1);
    if(lua_type(_L, -1) == LUA_TTABLE)
    {
        lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
        lua_gettable(_L, LUA_REGISTRYINDEX);
        lua_pushstring(_L, name.c_str());
        void** lib = (void**)lua_newuserdata(_L, sizeof(void*));
        *lib = nullptr;
        lua_pushvalue(_L, -4);

        lua_pushcfunction (_L, chimera::lua_ignore_newindex);
        lua_setfield(_L, -2, "__newindex");
        lua_pushstring(_L, chimera::typenames::TYPE_LIBRARY);
        lua_setfield(_L, -2, "__name");
        lua_pushinteger(_L, chimera::systemtypes::PID_LIBRARAY);
        lua_setfield(_L, -2, "__type");
        lua_setmetatable(_L, -2);
        lua_rawset(_L, -3);
        lua_pop(_L, 2);
    } else {
        lua_pop(_L, 1);
    }
}
