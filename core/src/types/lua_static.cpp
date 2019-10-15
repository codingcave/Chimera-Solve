#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
//#include <string.h>
#include <vector>
#include <list>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "Naming.hpp"
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
#include "ChimeraSystem.hpp"
#include "types/lua_static.hpp"

int chimera::lua_global_scope_newindex(lua_State* L)
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

int chimera::lua_ignore_newindex(lua_State* L)
{
    lua_settop(L, 0);
    return 0;
}

int chimera::lua_UserData_gc(lua_State* L)
{
    if(luaL_getmetafield(L, 1, "__type")) {
        int type = lua_tointeger(L, 2);
        void* value = *((void**)lua_touserdata(L, 1));

        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
        lua_gettable(L, LUA_REGISTRYINDEX);
        chimera::ChimeraSystem* chSys = (chimera::ChimeraSystem*)lua_touserdata(L, -1);
        if(value)
        {
            if(chSys->getTypeSystem()->isReferenced(value))
            {
                chSys->getTypeSystem()->releaseDependency(L, value);
            }
            else
            {
                chSys->getTypeSystem()->deleteValue(L, type, value);
            }
        }
    }
    return 0;
}
