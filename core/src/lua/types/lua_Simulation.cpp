#include <vector>
#include <iostream>
#include <unordered_map>
#include <list>
#include "lua.hpp"

#include "Naming.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "lua/LuaParser.hpp"
#include "lua/lua_static.hpp"
#include "LoggingSystem.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "interfaces/IEventListener.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "Simulation.hpp"
#include "lua/types/lua_Simulation.hpp"

void luat_Simulation_delete(lua_State* const L, void * const value, const int& type)
{
    delete (Simulation*)value;
}

int lua_Simulation_tostring(lua_State* L)
{
    lua_pushstring(L, Naming::Type_Simulation);
    return 1;
}

int lua_Simulation_index(lua_State* L)
{
	std::cout << luaL_typename(L, lua_upvalueindex(1)) << std::endl;
    std::cout << luaL_typename(L, 1) << std::endl;
    std::cout << luaL_typename(L, 2) << "  " << lua_tostring(L, 2) <<  std::endl;
    std::cout << lua_gettop(L) << std::endl;
    return 0;
}

int lua_Simulation_start(lua_State* L)
{
    lua_pushstring(L, Naming::Lua_reg_instance_LuaParser);
    lua_gettable(L, LUA_REGISTRYINDEX);
    LuaParser* lp = (LuaParser*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    lua_State* NL = (lua_State*)lua_touserdata(L, lua_upvalueindex(1));
    Simulation* sim = *((Simulation**)lua_touserdata(L, lua_upvalueindex(2)));
    if(!(sim->isRunning())) {
        //std::cout << "Resume: " << lua_gettop(NL) << " - " << lua_gettop(L) << std::endl;
        lp->_rL = NL;
        lua_resume(NL, L, 0);
        lp->_rL = L;

        //lua_pushnumber(L, sim->getStep());
        ParameterTypeSystem::pushValue({sim->getIntegrator()->getTimeType(), sim->getIntegrator()->currentTime()});

        //std::cout << "YIELD: " << lua_gettop(NL) << " - " << lua_gettop(L) << std::endl;
        return 1;
    }
    else
    {
        LoggingSystem::Error("Simulation is already running.");
        return 0;
    }
}

int lua_Simulation_stop(lua_State* L)
{
    Simulation* sim = *((Simulation**)lua_touserdata(L, lua_upvalueindex(2)));
    if(sim->isRunning())
    {
        sim->setYield(true);
    }
    else
    {
        LoggingSystem::Error("Simulation was not started yet.");
    }

    return 0;
}

int lua_Simulation_step(lua_State* L)
{

    return 0;
}

int k (lua_State *L, int status, lua_KContext ctx)
{
    std::cout << "before K" << std::endl;
    int result = lua_Simulation_run(L);
    std::cout << "after K" << std::endl;
    return result;
}

int lua_Simulation_run(lua_State* L)
{
    std::cout << "Simulation Run: " << lua_gettop(L) << std::endl;
    Simulation* sim = (Simulation*)lua_touserdata(L, lua_upvalueindex(1));
    //if(lua_isfunction
    sim->setYield(false);
    while(sim->nextStep())
    {
        lua_getglobal(L, "func");
        if(lua_isfunction(L, -1))
        {
            //lua_pushinteger(L, sim->getStep());
            //lua_pushinteger(L, 1);
            ParameterTypeSystem::pushValue({sim->getIntegrator()->getTimeType(), sim->getIntegrator()->currentTime()});
            ParameterTypeSystem::pushValue({sim->getIntegrator()->getStateType(), sim->getIntegrator()->currentState()});
            lua_pcall(L, 2, 0, 0);
            if(sim->getYield())
            {
                sim->setYield(false);
                lua_yieldk(L, 0, 0, k);
            }
            std::cout << "After yield" << std::endl;
        } else {
            lua_pop(L, 1);
        }
    }
    std::cout << "Simulation DONE" << std::endl;

    return 0;
}
