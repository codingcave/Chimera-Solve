#include <iostream>
#include <vector>
#include <unordered_map>
#include <complex>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "LoggingSystem.hpp"
#include "lua/types/vector/lua_vector_common.hpp"
#include "lua/lua_static.hpp"

void push_vector_library(lua_State* const L)
{
    void** lib = (void**)lua_newuserdata(L, sizeof(void*));
    *lib = nullptr;
    lua_newtable(L);
    lua_pushcfunction (L, lua_ignore_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushstring (L, "Library");
    lua_setfield(L, -2, "__name");
    lua_pushinteger (L, ParameterTypeSystem::pid_library);
    lua_setfield(L, -2, "__type");
    lua_pushcfunction (L, lua_vector_new);
    lua_setfield(L, -2, "__call");
    lua_pushstring(L, "__index");
    lua_newtable(L);
    lua_pushcfunction (L, lua_vector_base);
    lua_setfield(L, -2, "base");
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);
}

int lua_vector_new(lua_State* const L)
{
    if(lua_gettop(L) != 3)
    {
        LoggingSystem::Error("vector.new requires 2 arguments");
        return 0;
    }
    if(!lua_isinteger(L, 2))
    {
        LoggingSystem::Error("argument `1` to vector.new is not an integer number");
        return 0;
    }
    int a = lua_tointeger(L, 2);

    if(a <= 0)
    {
        LoggingSystem::Error("attempt to create invalid vector in vector.new (dimension <= 0)");
        return 0;
    }

    std::string meta("meta:");
    meta += Naming::Type_Vector;
    meta += "#";

    switch(lua_type(L, 3))
    {
    case LUA_TNUMBER:
        {
            meta += Naming::Type_real;
            break;
        }
    case LUA_TBOOLEAN:
        {
            meta += Naming::Type_boolean;
            break;
        }
    case LUA_TSTRING:
        {
            meta += Naming::Type_string;
            break;
        }
    case LUA_TUSERDATA:
        {
            if(luaL_getmetafield(L, 3, "__type"))
            {
                int type = lua_tointeger(L, -1);
                meta += ParameterTypeSystem::getParameterName(type);
            }
            break;
        }
    default:
        {
            LoggingSystem::Error("vector type is not supported");
            return 0;
        }
    }

    if(luaL_getmetatable(L, meta.c_str()))
    {
        if(lua_getfield(L, -1, "__new") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_call(L, 2, 1);
            return 1;
        }
    }
    LoggingSystem::Error("vector type is not supported");
    return 0;
}

int lua_vector_base(lua_State* const L)
{

}
