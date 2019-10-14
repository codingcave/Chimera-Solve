#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string.h>
#include <list>
#include <vector>
#include "lua.hpp"

#include "RuntimeNames.hpp"
#include "ExtensionNaming.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ModuleLoader.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/StateEventListener.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "simulation/TemporalStateEventProvider.hpp"
#include "simulation/AbstractSimulation.hpp"
#include "simulation/TemporalSimulation.hpp"
//#include "Simulation.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/types/lua_TemporalSimulation.hpp"
#include "ChimeraSystem.hpp"
#include "lua/lua_global.hpp"

// TODO (kekstoaster#2#): add dump function: dump content of a variable, since print only shows what that variable is usually (like vector - length): call meta __dump function if exists, otherwise tostring, otherwise leave it to lua

int chimera::runtime::lua_global_print(lua_State* L)
{
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        std::cout << luaL_tolstring(L, i, NULL) << " ";
        lua_pop(L, 1);
    }
    std::cout << std::endl;
    lua_settop(L, 0);
    return 0;
}

int chimera::runtime::lua_global_write(lua_State* L)
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

int chimera::runtime::lua_global_writeline(lua_State* L)
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


int chimera::runtime::lua_global_log(lua_State* L)
{
    if(lua_gettop(L) > 0) {
        std::string s(luaL_tolstring(L, 1, NULL));
        //LoggingSystem::Log(s);
    }
    return 0;
}

int chimera::runtime::lua_global_error(lua_State* L)
{
    if(lua_gettop(L) > 0) {
        std::string s(luaL_tolstring(L, 1, NULL));
        //LoggingSystem::Error(s);
    }
    return 0;
}

int chimera::runtime::lua_global_warning(lua_State* L)
{
    if(lua_gettop(L) > 0) {
        std::string s(luaL_tolstring(L, 1, NULL));
        //LoggingSystem::Warning(s);
    }
    return 0;
}

int chimera::runtime::lua_global_exit(lua_State* L)
{
    lua_close(L);
    exit(0);
    return 0;
}

int chimera::runtime::lua_global_simulation(lua_State* L)
{
    chimera::simulation::AbstractIntegrator* instance;
    chimera::ChimeraSystem* chSys;
    chimera::simulation::AbstractTemporalIntegrator* tempInt = nullptr;

    if(lua_gettop(L) > 0)
    {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
        lua_gettable(L, LUA_REGISTRYINDEX);
        chSys = (chimera::ChimeraSystem*)lua_touserdata(L, -1);

        if(lua_isuserdata(L, 1) &&
           luaL_getmetafield(L, 1, "__type") == LUA_TNUMBER &&
           chSys->getTypeSystem()->getParameterBase(lua_tonumber(L, -1)) == chimera::systemtypes::PID_INSTANCE &&
           luaL_getmetafield(L, 1, "__epptr") == LUA_TLIGHTUSERDATA &&
           chSys->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_integrator) == lua_touserdata(L, -1))
        {
            instance = *((chimera::simulation::AbstractIntegrator**)lua_touserdata(L, 1));
            tempInt = dynamic_cast<chimera::simulation::AbstractTemporalIntegrator*>(instance);
            if (!tempInt) { // extend with all other simulations
                return 0;
            }

            lua_pop(L, 3);
            vec_t_LuaItem items;
            for(int i = 2; i <= lua_gettop(L); i++)
            {
                items.push_back(chSys->getTypeSystem()->getValue(L, i));
            }
            instance->initialize(items);
        } else {
            //LoggingSystem::Error("Argument `1` to simulation is not supported.");
            return 0;
        }
    } else {
        //LoggingSystem::Error("function simulation needs first argument to be of type `integrator`.");
        return 0;
    }

    chimera::simulation::AbstractSimulation** sim = (chimera::simulation::AbstractSimulation**)lua_newuserdata(L, sizeof(chimera::simulation::AbstractSimulation*));
    lua_newtable(L);
    lua_State* NL = lua_newthread(L);

    *sim = nullptr;
    // case for all simulation types
    if (tempInt) {
        *sim = new TemporalSimulation(tempInt);
        lua_pushliteral(L, "temporal");
        lua_setfield(L, -3, "__simtype");
        lua_pushlightuserdata(NL, (void*)*sim);
        lua_pushcclosure(NL, lua_TemporalSimulation_run, 1);
    }

    lua_setfield(L, -2, "__thread");
    lua_pushcfunction (L, lua_ignore_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushstring(L, chimera::simulation::Naming::Type_Simulation);
    lua_setfield(L, -2, "__name");
    lua_pushinteger(L, chSys->getTypeSystem()->getParameterID(chimera::simulation::Naming::Type_Simulation));
    lua_setfield(L, -2, "__type");
    lua_pushcfunction (L, lua_Simulation_tostring);
    lua_setfield(L, -2, "__tostring");

    // index table with all associated Lua values inEntryPoint
    lua_pushstring(L, "__index");
    lua_newtable(L);

    // case for all simulation types
    if (tempInt) {
        lua_pushlightuserdata(L, (void*)NL);
        lua_pushvalue(L, -5);
        lua_pushcclosure(L, lua_TemporalSimulation_start, 2);
        lua_setfield(L, -2, "start");
        lua_pushlightuserdata(L, (void*)NL);
        lua_pushvalue(L, -5);
        lua_pushcclosure(L, lua_TemporalSimulation_stop, 2);
        lua_setfield(L, -2, "stop");
        lua_pushlightuserdata(L, (void*)NL);
        lua_pushvalue(L, -5);
        lua_pushcclosure(L, lua_TemporalSimulation_step, 2);
        lua_setfield(L, -2, "step");
        lua_pushvalue(L, -4);
        lua_pushcclosure(L, lua_TemporalSimulation_current, 1);
        lua_setfield(L, -2, "state");
        lua_pushvalue(L, -4);
        lua_pushcclosure(L, lua_TemporalSimulation_onStep, 1);
        lua_setfield(L, -2, "onstep");
    }
    //lua_pushcclosure(L, lua_Simulation_add_event, 1);
    //lua_setfield(L, -2, "observe");
    //lua_pushcclosure(L, lua_Simulation_register, 2);
    //lua_setfield(L, -2, "register");
    // create lookup for dynamic methods like events
    /*lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -6);
    lua_pushcclosure (L, lua_TemporalSimulation_index, 1);
    */
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);

    return 1;
}

int chimera::runtime::lua_global_scope_newindex(lua_State* L)
{
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
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

int chimera::runtime::lua_ignore_newindex(lua_State* L)
{
    lua_settop(L, 0);
    return 0;
}

int chimera::runtime::lua_argv_len(lua_State* L)
{
    lua_pushstring(L, chimera::runtime::Naming::Lua_reg_argv);
    lua_gettable(L, LUA_REGISTRYINDEX);
    int l = lua_rawlen(L, -1);
    lua_settop(L, 0);
    lua_pushinteger (L, l);
    return 1;
}
