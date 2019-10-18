#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "Naming.hpp"
//#include "interfaces/EventHandler/IEntryPointEventHandler.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "types/lua_static.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "types/lua_EntryPoint.hpp"
#include "ChimeraSystem.hpp"

void chimera::ChimeraSystem::pushEntryPoint(const std::string& name, EntryPoint* ep)
{
    int top = lua_gettop(_L);
    ep->init(_L);

    // save entrypoint also in references
    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_rawget(_L, LUA_REGISTRYINDEX);
    // get system names
    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_rawget(_L, LUA_REGISTRYINDEX);
    // if new EntryPoint conflicts with existing name
    lua_pushstring(_L, name.c_str());
    if(!lua_rawget(_L, -2))
    {
        // pop nil
        lua_pop(_L, 1);
        // push new EntryPoint
        lua_pushstring(_L, name.c_str());
        EntryPoint **lep = (EntryPoint **)lua_newuserdata(_L, sizeof(EntryPoint *));
        *lep = ep;

        // metatable
        lua_newtable(_L);
        lua_pushcfunction (_L, chimera::lua_ignore_newindex);
        lua_setfield(_L, -2, "__newindex");
        lua_pushcfunction (_L, chimera::lua_EntryPoint_call);
        lua_setfield(_L, -2, "__call");
        lua_pushcfunction (_L, chimera::lua_EntryPoint_len);
        lua_setfield(_L, -2, "__len");
        lua_pushcfunction (_L, chimera::lua_EntryPoint_tostring);
        lua_setfield(_L, -2, "__tostring");
        lua_pushstring(_L, chimera::luanames::LUA_NAME_ENTRYPOINT);
        lua_setfield(_L, -2, "__name");
        lua_pushinteger(_L, chimera::systemtypes::PID_ENTRYPOINT);
        lua_setfield(_L, -2, "__type");
        lua_pushlstring(_L, name.c_str(), name.size());
        lua_setfield(_L, -2, "__ep");
        lua_pushlightuserdata(_L, ep);
        lua_setfield(_L, -2, "__ptr");
        lua_newtable(_L);
        lua_setfield(_L, -2, "__loaded");

        // index table with all associated Lua values inEntryPoint
        lua_pushstring(_L, "__index");
        lua_newtable(_L);

        for(auto it = ep->beginItems(); it != ep->endItems(); it++)
        {
            lua_pushlstring(_L, it->first.c_str(), it->first.size());
            pushLuaValue(_L, it->second.getType(), it->second.getValue());
            lua_rawset(_L, -3);
        }
        lua_rawset(_L, -3);

        // set individual meta table
        lua_setmetatable(_L, -2);
        // set entrypoint also in references
        lua_pushvalue(_L, -1);
        lua_rawsetp(_L, -5, ep);
        // name value pair in system names
        lua_rawset(_L, -3);
    }
    lua_settop(_L, top);
}

void chimera::ChimeraSystem::popEntryPoint(const std::string& name, EntryPoint* ep)
{
return;
/*
    int top = lua_gettop(_L);
    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_rawget(_L, LUA_REGISTRYINDEX);
    // if new EntryPoint conflicts with existing name
    if(lua_rawgetp(_L, -1, ep))
    {
        if(luaL_getmetafield(_L, -1, "__ep") == LUA_TSTRING)
        {
            std::string givenName = lua_tostring(_L, -1);
            lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
            lua_rawget(_L, LUA_REGISTRYINDEX);
            lua_pushvalue(_L, -2);
            lua_pushnil(_L);
            lua_pushvalue(_L, -2);
            lua_pushnil(_L);
            lua_rawset(_L, -5);
            lua_rawset(_L, -6);
            lua_pop(_L, 2);
        }

        _typeSys->removeParameter(ep);
        EntryPoint **lep = (EntryPoint **)lua_touserdata(_L, -1);
        *lep = nullptr;
    }
    lua_settop(_L, top);
*/
}

int chimera::lua_EntryPoint_call(lua_State* L)
{
    if(lua_gettop(L) == 2)
    {
        if(lua_isstring(L, 2))
        {
            const char *name = lua_tostring(L, 2);

            if(luaL_getmetafield(L, 1, "__loaded"))
            {
                lua_pushvalue(L, 2);
                if(!lua_rawget(L, -2))
                {
                    chimera::EntryPoint* ep = *((chimera::EntryPoint**)lua_touserdata(L, 1));
                    chimera::Module* module = ep->_chimeraSystem->includeModule(ep, name);
                    if(module)
                    {
                        module->loadModule(ep, name, ep->getModuleLoadParams());
                        lua_getfield(L, 3, name);
                        return 1;
                    }
                }
                else
                {
                    return 1;
                }
            }
            else
            {

            }
        }
        else
        {
            //LoggingSystem::Error("Cannot load module. Supplied value is not a string.");
        }
    }
    else
    {
        //LoggingSystem::Error("Cannot load module. Too many arguments.");
    }
    lua_pushnil(L);
    return 1;
}

int chimera::lua_EntryPoint_len(lua_State* L)
{
    EntryPoint* ep = *((EntryPoint**)lua_touserdata(L, 1));
    lua_pushinteger(L, ep->size());
    return 1;
}

int chimera::lua_EntryPoint_tostring(lua_State* L)
{
    const EntryPoint *ep = *((const EntryPoint **)lua_touserdata(L, -1));
    if(ep)
    {
        lua_pushstring(L, ep->getGUID().c_str());
    }
    else
    {
        std::string txt(chimera::luanames::LUA_NAME_ENTRYPOINT);
        txt += "<RefError>";
        lua_pushstring(L, txt.c_str());
    }
    return 1;
}
