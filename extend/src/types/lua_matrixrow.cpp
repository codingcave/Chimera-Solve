#include <boost/numeric/ublas/matrix.hpp>
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

#include "types/lua_matrixrow.hpp"

void luat_matrixrow_init(lua_State* const L, const int& type)
{
    std::string tn(ParameterTypeSystem::getParameterName(type));
    std::string tnb(ParameterTypeSystem::getParameterName(ParameterTypeSystem::getParameterBase(type)));
    std::string tnt(ParameterTypeSystem::getParameterName(ParameterTypeSystem::getParameterTag(type)));
    lua_pushcfunction (L, lua_matrixrow_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction (L, lua_matrixrow_len);
    lua_setfield(L, -2, "__len");
    lua_pushcfunction (L, lua_matrixrow_iterator);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction (L, lua_matrixrow_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction (L, lua_matrixrow_index);
    lua_setfield(L, -2, "__index");
    std::string meta("meta:");
    meta += Naming::Type_Matrix;
    meta += "#";
    meta += ParameterTypeSystem::getParameterName(ParameterTypeSystem::getParameterTag(type));
    if(luaL_getmetatable(L, meta.c_str()))
    {
        if(lua_getfield(L, -1, "__get"))
        {
            lua_setfield(L, -4, "__get");
        }
        else
        {
            lua_pop(L, 1);
        }

        if(lua_getfield(L, -1, "__set"))
        {
            lua_setfield(L, -4, "__set");
        }
        else
        {
            lua_pop(L, 1);
        }
    }
}

void luat_matrixrow_delete(lua_State* const L, void * const value, const int& type)
{
    delete (struct T_MatrixRowDef*)value;
}

int lua_matrixrow_newindex(lua_State* const L)
{
    struct T_MatrixRowDef* m = *((struct T_MatrixRowDef **)lua_touserdata(L, 1));
    int i = lua_tointeger(L, 2);
    if(i >= 0 && i < m->length)
    {
        if(luaL_getmetafield(L, 1, "__set") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 1);
            lua_pushinteger(L, m->row);
            lua_pushinteger(L, i);
            lua_pushvalue(L, 3);
            lua_pcall(L, 4, 0, 0);
        }
        else
        {
            LoggingSystem::Error("no set method defined");
        }
    }
    else
    {
        LoggingSystem::Error("Index out of bounds in matrix set");
    }
    return 0;
}

int lua_matrixrow_iterator(lua_State* const L)
{
    if(lua_gettop(L) >= 3)
    {
        struct T_MatrixRowDef* m = *((struct T_MatrixRowDef **)lua_touserdata(L, 1));
        int i = 0;
        if(lua_isnil(L, 3)) {
            i = 0;
        } else if(lua_isinteger(L, 3)) {
            i = lua_tointeger(L, 3) + 1;
        } else {
            return 0;
        }
        if(i < m->length)
        {
            lua_pushinteger(L, i);

            if(luaL_getmetafield(L, 1, "__get") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 1);
                lua_pushinteger(L, m->row);
                lua_pushinteger(L, i);
                lua_pcall(L, 3, 1, 0);
            }
            else
            {
                LoggingSystem::Error("Matrix has no associated get method");
                return 0;
            }

            return 2;
        }
    }
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

int lua_matrixrow_len(lua_State* const L)
{
    struct T_MatrixRowDef* m = *((struct T_MatrixRowDef **)lua_touserdata(L, 1));
    lua_pushinteger(L, m->length);
    return 1;
}

int lua_matrixrow_tostring(lua_State* const L)
{
    struct T_MatrixRowDef* m = *((struct T_MatrixRowDef **)lua_touserdata(L, 1));

    std::string text(Naming::Type_MatrixRow);
    if(luaL_getmetafield(L, 1, "__type"))
    {
        int itemtype = ParameterTypeSystem::getParameterTag(lua_tointeger(L, -1));
        text += "<";
        text += ParameterTypeSystem::getParameterName(itemtype);
        text += ">";
    }
    text += "(";
    text += std::to_string(m->length);
    text += ")";
    lua_pushlstring(L, text.c_str(), text.size());
    return 1;
}

int lua_matrixrow_index(lua_State* const L)
{
    struct T_MatrixRowDef* m = *((struct T_MatrixRowDef **)lua_touserdata(L, 1));
    int i = lua_tointeger(L, 2);
    if(i >= 0 && i < m->length)
    {
        if(luaL_getmetafield(L, 1, "__get") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 1);
            lua_pushinteger(L, m->row);
            lua_pushinteger(L, i);
            lua_pcall(L, 3, 1, 0);
            return 1;
        }
        else
        {
            LoggingSystem::Error("no get method defined");
        }
    }
    else
    {
        LoggingSystem::Error("Index out of bounds in matrix get");
    }
    return 0;
}
