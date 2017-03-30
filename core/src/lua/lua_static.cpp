#include <iostream>
#include <unordered_map>
#include <string.h>
#include <list>
#include "lua.hpp"

#include "Naming.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "EntryPoint.hpp"
#include "ModuleLoader.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/StateEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "Simulation.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/lua_static.hpp"

int lua_global_print(lua_State* L)
{
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        std::cout << luaL_tolstring(L, i, NULL) << " ";
        lua_pop(L, 1);
        /*
        if(lua_getmetatable(L, i)) {
            lua_getfield(L, -1, "__type");
            std::cout << "[" << lua_tonumber(L, -1) << "]" << " ";
            lua_pop(L, 2);
        }
        */
    }
    std::cout << std::endl;
    lua_settop(L, 0);
    return 0;
}

int lua_global_write(lua_State* L)
{
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        std::cout << luaL_tolstring(L, i, NULL);
        lua_pop(L, 1);
    }
    std::cout << std::flush;
    lua_settop(L, 0);
    return 0;
}

int lua_global_writeline(lua_State* L)
{
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        std::cout << luaL_tolstring(L, i, NULL);
        lua_pop(L, 1);
    }
    std::cout << std::endl;
    lua_settop(L, 0);
    return 0;
}


int lua_global_log(lua_State* L)
{
    if(lua_gettop(L) > 0) {
        std::string s(luaL_tolstring(L, 1, NULL));
        LoggingSystem::Log(s);
    }
    return 0;
}

int lua_global_error(lua_State* L)
{
    if(lua_gettop(L) > 0) {
        std::string s(luaL_tolstring(L, 1, NULL));
        LoggingSystem::Error(s);
    }
    return 0;
}

int lua_global_warning(lua_State* L)
{
    if(lua_gettop(L) > 0) {
        std::string s(luaL_tolstring(L, 1, NULL));
        LoggingSystem::Warning(s);
    }
    return 0;
}

int lua_global_exit(lua_State* L)
{
    lua_close(L);
    exit(0);
    return 0;
}

int lua_global_simulation(lua_State* L)
{
    AbstractIntegrator* instance;
    if(lua_gettop(L) > 0)
    {
        if(lua_isuserdata(L, 1) &&
           luaL_getmetafield(L, 1, "__type") &&
           ParameterTypeSystem::getParameterBase(lua_tonumber(L, -1)) == ParameterTypeSystem::getParameterID(Naming::Type_Instance) &&
           luaL_getmetafield(L, 1, "__ep") &&
           Naming::EntryPoint_integrator == std::string(lua_tostring(L, -1)))
        {
            instance = *((AbstractIntegrator**)lua_touserdata(L, 1));
            lua_pop(L, 2);
            vec_t_LuaItem items;
            for(int i = 2; i <= lua_gettop(L); i++)
            {
                items.push_back(ParameterTypeSystem::getValue(i));
                lua_pop(L, 1);
            }
            instance->initialize(items);
        } else {
            LoggingSystem::Error("Argument `1` to simulation is not supported.");
            return 0;
        }
    } else {
        LoggingSystem::Error("function simulation needs first argument to be of type `integrator`.");
        return 0;
    }

    Simulation** sim = (Simulation**)lua_newuserdata(L, sizeof(Simulation*));
    *sim = new Simulation(instance);
    (*sim)->registerEvent("step", instance->createStateEventManager());
    lua_newtable(L);
    lua_State* NL = lua_newthread(L);
    lua_pushlightuserdata(NL, (void*)*sim);
    lua_pushcclosure(NL, lua_Simulation_run, 1);
    lua_setfield(L, -2, "__thread");
    lua_pushlightuserdata(L, (void*)NL);
    lua_setfield(L, -2, "__luastate");
    lua_pushcfunction (L, lua_ignore_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushstring(L, Naming::Lua_name_Simulation);
    lua_setfield(L, -2, "__name");
    lua_pushinteger(L, ParameterTypeSystem::getParameterID(Naming::Type_Simulation));
    lua_setfield(L, -2, "__type");
    lua_pushcfunction (L, lua_Simulation_tostring);
    lua_setfield(L, -2, "__tostring");

    // index table with all associated Lua values inEntryPoint
    lua_pushstring(L, "__index");
    lua_newtable(L);
    lua_pushlightuserdata(L, (void*)NL);
    lua_pushvalue(L, -5);
    lua_pushcclosure(L, lua_Simulation_start, 2);
    lua_setfield(L, -2, "start");
    lua_pushlightuserdata(L, (void*)NL);
    lua_pushvalue(L, -5);
    lua_pushcclosure(L, lua_Simulation_stop, 2);
    lua_setfield(L, -2, "stop");
    lua_pushlightuserdata(L, (void*)NL);
    lua_pushvalue(L, -5);
    lua_pushcclosure(L, lua_Simulation_step, 2);
    lua_setfield(L, -2, "step");
    //lua_pushcclosure(L, lua_Simulation_register, 2);
    //lua_setfield(L, -2, "register");
    // create lookup for dynamic methods like events
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -6);
    lua_pushcclosure (L, lua_Simulation_index, 1);
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);

    lua_rawset(L, -3);
    // set individual meta table
    lua_setmetatable(L, -2);
    // name value pair in system names

    return 1;
}

int lua_global_scope_newindex(lua_State* L)
{
    lua_pushstring(L, Naming::Lua_reg_systemNames);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 2);
    lua_gettable(L, -2);
    if(lua_isnoneornil(L, -1))
    {
        lua_settop(L, 3);
        lua_rawset(L, -3);
    }

    lua_settop(L, 0);
    return 0;
}

int lua_ignore_newindex(lua_State* L)
{
    lua_settop(L, 0);
    return 0;
}

int lua_argv_len(lua_State* L)
{
    lua_pushstring(L, Naming::Lua_reg_argv);
    lua_gettable(L, LUA_REGISTRYINDEX);
    int l = lua_rawlen(L, -1);
    lua_settop(L, 0);
    lua_pushinteger (L, l);
    return 1;
}
