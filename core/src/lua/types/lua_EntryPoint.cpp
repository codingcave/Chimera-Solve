#include <iostream>
#include <vector>
#include <unordered_map>
#include "lua.hpp"

#include "Naming.hpp"
//#include "interfaces/EventHandler/IEntryPointEventHandler.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
//#include "Registry.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ModuleLoader.hpp"
#include "lua/lua_static.hpp"
#include "lua/types/lua_EntryPoint.hpp"
#include "lua/LuaParser.hpp"
#include "LoggingSystem.hpp"

void LuaParser::pushEntryPoint(EntryPoint* ep)
{
    ep->init(_L);
    std::string typeName(Naming::Lua_name_EntryPoint);
    typeName += "/";
    typeName += ep->getName();
    int type = ParameterTypeSystem::registerParameter(typeName, {nullptr, nullptr, nullptr}, ParameterTypeSystem::getParameterID(Naming::Type_EntryPoint), 0, false);

    // get system names
    lua_pushstring(_L, Naming::Lua_reg_systemNames);
    lua_rawget(_L, LUA_REGISTRYINDEX);
    // if new EntryPoint conflicts with existing name
    lua_pushstring(_L, ep->getName().c_str());
    if(!lua_rawget(_L, -2))
    {
        // pop nil
        lua_pop(_L, 1);
        // push new EntryPoint
        lua_pushstring(_L, ep->getName().c_str());
        EntryPoint **lep = (EntryPoint **)lua_newuserdata(_L, sizeof(EntryPoint *));
        *lep = ep;

        // metatable
        lua_newtable(_L);
        lua_pushcfunction (_L, lua_ignore_newindex);
        lua_setfield(_L, -2, "__newindex");
        lua_pushcfunction (_L, lua_EntryPoint_call);
        lua_setfield(_L, -2, "__call");
        lua_pushcfunction (_L, lua_EntryPoint_len);
        lua_setfield(_L, -2, "__len");
        lua_pushcfunction (_L, lua_EntryPoint_tostring);
        lua_setfield(_L, -2, "__tostring");
        lua_pushstring(_L, Naming::Lua_name_EntryPoint);
        lua_setfield(_L, -2, "__name");
        lua_pushinteger(_L, type);
        lua_setfield(_L, -2, "__type");
        lua_pushlstring(_L, ep->getName().c_str(), ep->getName().size());
        lua_setfield(_L, -2, "__ep");
        lua_newtable(_L);
        lua_setfield(_L, -2, "__loaded");

        // index table with all associated Lua values inEntryPoint
        lua_pushstring(_L, "__index");
        lua_newtable(_L);

        for(auto it = ep->beginItems(); it != ep->endItems(); it++)
        {
            lua_pushlstring(_L, it->first.c_str(), it->first.size());
            pushLuaValue(it->second);
            lua_rawset(_L, -3);
        }
        lua_rawset(_L, -3);

        // set individual meta table
        lua_setmetatable(_L, -2);
        // name value pair in system names
        lua_rawset(_L, -3);
    }
    lua_settop(_L, 0);
}

int lua_EntryPoint_call(lua_State* L)
{
    if(lua_gettop(L) == 2)
    {
        if(lua_isstring(L, 2))
        {
            const char *name = lua_tostring(L, 2);

            lua_getmetatable(L, 1);
            lua_pushlstring(L, "__loaded", 8);
            lua_rawget(L, -2);
            lua_pushstring(L, name);
            if(!lua_rawget(L, -2))
            {
                lua_pushstring(L, Naming::Lua_reg_instance_ModuleLoader);
                lua_rawget(L, LUA_REGISTRYINDEX);
                ModuleLoader* loader = ((ModuleLoader*)lua_touserdata(L, -1));
                EntryPoint* ep = *((EntryPoint**)lua_touserdata(L, 1));
                Registry* reg = loader->Load(name);
                if(reg)
                {
                    if(ep->load(name, reg))
                    {
                        lua_pushstring(L, Naming::Lua_reg_systemNames);
                        lua_gettable(L, LUA_REGISTRYINDEX);
                        lua_pushlstring(L, ep->getName().c_str(), ep->getName().size());
                        lua_rawget(L, -2);
                        luaL_getmetafield(L, -1, "__loaded");
                        lua_getfield(L, -1, name);
                        lua_insert(L, 1);
                        lua_settop(L, 1);
                        return 1;
                    }
                    else
                    {
                        loader->Unload(name);
                    }
                }
            }
            else
            {
                lua_insert(L, 1);
                lua_settop(L, 1);
                return 1;
            }
        }
        else
        {
            LoggingSystem::Error("Cannot load module. Supplied value is not a string.");
        }
    }
    else
    {
        LoggingSystem::Error("Cannot load module. Too many arguments.");
    }
    lua_settop(L, 0);
    lua_settop(L, 1);
    return 1;
}

int lua_EntryPoint_len(lua_State* L)
{
    EntryPoint* ep = *((EntryPoint**)lua_touserdata(L, 1));
    lua_settop(L, 0);
    lua_pushinteger(L, ep->size());
    return 1;
}

int lua_EntryPoint_tostring(lua_State* L)
{
    const EntryPoint *ep = *((const EntryPoint **)lua_touserdata(L, -1));
    std::string txt(ep->getName());

    lua_pushstring(L, txt.c_str());
    return 1;
}
