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
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "ItemContainer.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "Registry.hpp"
#include "EntryPointBase/OutputRegistry.hpp"
#include "EntryPointBase/OutputMultiRegistry.hpp"
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

int lua_Simulation_register_event(lua_State* L)
{
    static const size_t basetype = ParameterTypeSystem::getParameterID(Naming::Type_Instance);
    static const size_t tagtype = ParameterTypeSystem::getParameterID(std::string(Naming::Lua_name_EntryPoint) + std::string("/") + std::string(Naming::EntryPoint_output));

    if(lua_gettop(L) > 0 && luaL_getmetafield(L, 1, "__type"))
    {
        int type = lua_tointeger(L, -1);
        if(ParameterTypeSystem::getParameterBase(type) == basetype && ParameterTypeSystem::getParameterTag(type) == tagtype)
        {
            const std::string typeName = ParameterTypeSystem::getParameterName(type);
            std::size_t found;
            found = typeName.find("/");
            if(found != std::string::npos)
            {
                found = typeName.find("/", found + 1);
                std::string regName = typeName.substr(found + 1);
                EntryPoint* ep = EntryPointSystem::get(Naming::EntryPoint_output);
                const Registry* reg = ep->getRegistry(regName);
                if(reg)
                {
                    Simulation* sim = *((Simulation**)lua_touserdata(L, lua_upvalueindex(1)));

                    const OutputRegistry* reg_single = dynamic_cast<const OutputRegistry*>(reg);
                    if(reg_single)
                    {
                        IEventListener* listen = *((IEventListener**)lua_touserdata(L, 1));
                        if(listen)
                        {
                            sim->addListener(lua_tostring(L, lua_upvalueindex(2)), listen);
                        }
                        return 0;
                    }

                    const OutputMultiRegistry* reg_multi = dynamic_cast<const OutputMultiRegistry*>(reg);
                    if(reg_multi)
                    {
                        IEventListenerProvider* provider = *((IEventListenerProvider**)lua_touserdata(L, 1));
                        if(provider)
                        {
                            sim->addListener(lua_tostring(L, lua_upvalueindex(2)), provider);
                        }
                        return 0;
                    }
                }
            }

            return 0;
        }

        LoggingSystem::Error("Parameter `1` is not a valid event listener");
        return 0;
    }
    LoggingSystem::Error("Expected parameter `1` to be event listener ");
}

int lua_Simulation_index(lua_State* L)
{
    Simulation* sim = *((Simulation**)lua_touserdata(L, lua_upvalueindex(1)));
    std::string fname = lua_tostring(L, 2);
    if(fname.size() > 2 && fname[0] == 'o' && fname[1] == 'n')
    {
        std::string ename = fname.substr(2);

        if(sim->hasEvent(ename))
        {
            lua_pushvalue(L, lua_upvalueindex(1));
            lua_pushstring(L, ename.c_str());
            lua_pushcclosure (L, lua_Simulation_register_event, 2);
            return 1;
        }
    }


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
        vec_t_LuaItem items;
        for(int i = 1; i <= lua_gettop(L); i++)
        {
            items.push_back(ParameterTypeSystem::getValue(i));
        }
        sim->getIntegrator()->start(items);

        lp->_rL = NL;
        lua_resume(NL, L, 0);
        lp->_rL = L;

        //lua_pushnumber(L, sim->getStep());
        ParameterTypeSystem::pushValue({sim->getIntegrator()->getTimeType(), sim->getIntegrator()->currentTime()});
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
    int result = lua_Simulation_run(L);
    return result;
}

int lua_Simulation_run(lua_State* L)
{
    Simulation* sim = (Simulation*)lua_touserdata(L, lua_upvalueindex(1));
    //if(lua_isfunction
    sim->setYield(false);
    while(sim->nextStep())
    {
        if(sim->getEventManager("step"))
            sim->getEventManager("step")->trigger(sim, nullptr);
    }

        //lua_getglobal(L, "func");
        //if(lua_isfunction(L, -1))

        if(false)
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

        } else {
            lua_pop(L, 1);
        }

    return 0;
}
