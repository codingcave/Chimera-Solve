#include <boost/numeric/ublas/matrix.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <complex>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "LoggingSystem.hpp"

#include "types/lua_matrix.hpp"

namespace ublas = boost::numeric::ublas;

//const std::string matrixRealMetaName = (std::string("meta:") + std::string(Naming::Type_Complex));
//const std::string matrixComplexMetaName = (std::string("meta:") + std::string(Naming::Type_Complex));
const std::string matrixMetaName = (std::string("meta:") + std::string(Naming::Type_Matrix));
const std::string matrixrowMetaName = (std::string("meta:") + std::string(Naming::Type_MatrixRow));

void luat_matrix_init(lua_State* const L, const int& type)
{
    lua_pushcfunction (L, lua_matrix_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction (L, lua_matrix_length);
    lua_setfield(L, -2, "__len");
    lua_pushcfunction (L, lua_matrix_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction (L, lua_matrix_eq);
    lua_setfield(L, -2, "__eq");
    lua_pushcfunction (L, lua_matrix_iterator);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction (L, lua_matrix_add);
    lua_setfield(L, -2, "__add");
    lua_pushcfunction (L, lua_matrix_sub);
    lua_setfield(L, -2, "__sub");
    lua_pushcfunction (L, lua_matrix_mul);
    lua_setfield(L, -2, "__mul");
    lua_pushcfunction (L, lua_matrix_div);
    lua_setfield(L, -2, "__div");
    lua_pushcfunction (L, lua_matrix_mod);
    lua_setfield(L, -2, "__mod");
    lua_pushcfunction (L, lua_matrix_pow);
    lua_setfield(L, -2, "__pow");
    lua_pushcfunction (L, lua_matrix_unm);
    lua_setfield(L, -2, "__unm");
    lua_pushcfunction (L, lua_matrix_idiv);
    lua_setfield(L, -2, "__idiv");
    lua_pushcfunction (L, lua_matrix_band);
    lua_setfield(L, -2, "__band");
    lua_pushcfunction (L, lua_matrix_bor);
    lua_setfield(L, -2, "__bor");
    lua_pushcfunction (L, lua_matrix_bxor);
    lua_setfield(L, -2, "__bxor");
    lua_pushcfunction (L, lua_matrix_bnot);
    lua_setfield(L, -2, "__bnot");
    lua_pushcfunction (L, lua_matrix_shl);
    lua_setfield(L, -2, "__shl");
    lua_pushcfunction (L, lua_matrix_shr);
    lua_setfield(L, -2, "__shr");

    lua_pushcfunction (L, lua_matrix_index);
    lua_setfield(L, -2, "__index");
    map_t_LuaMethods* matrixMethods = new map_t_LuaMethods();
    (*matrixMethods)["transpose"] = lua_matrix_transpose;
    (*matrixMethods)["all"] = lua_matrix_all;
    (*matrixMethods)["any"] = lua_matrix_any;
    (*matrixMethods)["foreach"] = lua_matrix_foreach;
    (*matrixMethods)["copy"] = lua_matrix_copy;
    (*matrixMethods)["get"] = lua_matrix_get;
    (*matrixMethods)["set"] = lua_matrix_set;

    lua_pushlightuserdata(L, (void*)matrixMethods);
    lua_setfield(L, -2, "__methods");
}

int lua_matrix_iterator(lua_State* const L)
{
    if(lua_gettop(L) >= 3)
    {
        struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, 1));
        int i = 0;
        if(lua_isnil(L, 3)) {
            i = 0;
        } else if(lua_isinteger(L, 3)) {
            i = lua_tointeger(L, 3) + 1;
        } else {
            return 0;
        }

        if(i < m->rows)
        {
            if(luaL_getmetafield(L, 1, "__type"))
            {
                int itemtype = ParameterTypeSystem::getParameterTag(lua_tointeger(L, -1));
                lua_pushinteger(L, i);
                struct T_MatrixRowDef** mrow = (struct T_MatrixRowDef **)lua_newuserdata(L, sizeof(struct T_MatrixRowDef*));
                *mrow = new struct T_MatrixRowDef({i, m->cols, m->readonly, m->value});
                luaL_setmetatable(L, ((matrixrowMetaName + "#") + ParameterTypeSystem::getParameterName(itemtype)).c_str());
                return 2;
            }
        }
    }
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

int lua_matrix_length(lua_State* const L)
{
    struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, 1));
    lua_pushinteger(L, m->rows);
    return 1;
}

int lua_matrix_tostring(lua_State* const L)
{
    struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, 1));

    std::string text(Naming::Type_Matrix);
    if(luaL_getmetafield(L, 1, "__type"))
    {
        int itemtype = ParameterTypeSystem::getParameterTag(lua_tointeger(L, -1));
        text += "<";
        text += ParameterTypeSystem::getParameterName(itemtype);
        text += ">";
    }
    text += "(";
    text += std::to_string(m->rows);
    text += ",";
    text += std::to_string(m->cols);
    text += ")";
    lua_pushlstring(L, text.c_str(), text.size());
    return 1;
}

int lua_matrix_index(lua_State* const L)
{
    switch(lua_type(L, 2))
    {
    case LUA_TSTRING:
        {
            struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, 1));
            std::string item(lua_tostring(L, 2));
            if(item == "size")
            {
                lua_pushnumber(L, m->rows);
                lua_pushnumber(L, m->cols);
                return 2;
            }
            else if(item == "readonly")
            {
                lua_pushboolean(L, m->readonly);
                return 1;
            }
            else
            {
                if(m->readonly && item == "set"){
                    LoggingSystem::Error("set method is not allowed for readonly matrices");
                    return 0;
                }
                if(luaL_getmetafield(L, 1, "__methods"))
                {
                    map_t_LuaMethods* matrixMethods = (map_t_LuaMethods*)lua_touserdata(L, -1);
                    map_t_LuaMethods::const_iterator available = matrixMethods->find (item);
                    if(available != matrixMethods->end())
                    {
                        lua_pushvalue(L, 1);
                        lua_pushcclosure(L, available->second, 1);
                        return 1;
                    }
                }
            }
        }
    case LUA_TNUMBER:
        {
            if(lua_isinteger(L, 2))
            {
                struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, 1));
                int i = lua_tointeger(L, 2);
                if(i >= 0 && i < m->rows)
                {
                    if(luaL_getmetafield(L, 1, "__type"))
                    {
                        int itemtype = ParameterTypeSystem::getParameterTag(lua_tointeger(L, -1));
                        struct T_MatrixRowDef** mrow = (struct T_MatrixRowDef **)lua_newuserdata(L, sizeof(struct T_MatrixRowDef*));
                        *mrow = new struct T_MatrixRowDef({i, m->cols, m->readonly, m->value});
                        std::string mmm((matrixrowMetaName + "#") + ParameterTypeSystem::getParameterName(itemtype));
                        luaL_setmetatable(L, mmm.c_str());
                        return 1;
                    }
                }
            }
            break;
        }
    }
    LoggingSystem::Error("no valid index lookup for matrix");
    return 0;
}

int lua_matrix_newindex(lua_State* const L)
{
    // ignore
    lua_settop(L, 0);
    return 0;
}


int lua_matrix_eq(lua_State* const L)
{
    if(luaL_getmetafield(L, 1, "__type"))
    {
        if(ParameterTypeSystem::getParameterBase(lua_tointeger(L, -1)) != (unsigned)ParameterTypeSystem::pid_matrix)
        {
            lua_pushboolean(L, false);
            return 1;
        }
        lua_pop(L, 1);
    }
    else
    {
        lua_pushboolean(L, false);
        return 1;
    }

    if(luaL_getmetafield(L, 2, "__type"))
    {
        if(ParameterTypeSystem::getParameterBase(lua_tointeger(L, -1)) != (unsigned)ParameterTypeSystem::pid_matrix)
        {
            lua_pushboolean(L, false);
            return 1;
        }
        lua_pop(L, 1);
    }
    else
    {
        lua_pushboolean(L, false);
        return 1;
    }

    struct T_MatrixDef* m1 = *((struct T_MatrixDef**)lua_touserdata(L, 1));
    struct T_MatrixDef* m2 = *((struct T_MatrixDef**)lua_touserdata(L, 2));

    if(m1->rows != m2->rows || m1->cols != m2->cols)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    for(int i = 0; i < m1->rows; i++)
    {
        for(int j = 0; j < m1->cols; j++)
        {
            luaL_getmetafield(L, 1, "__get");
            lua_pushvalue(L, 1);
            lua_pushinteger(L, i);
            lua_pushinteger(L, j);
            lua_pcall(L, 3, 1, 0);

            luaL_getmetafield(L, 2, "__get");
            lua_pushvalue(L, 2);
            lua_pushinteger(L, i);
            lua_pushinteger(L, j);
            lua_pcall(L, 3, 1, 0);

            if(!lua_compare(L, -1, -2, LUA_OPEQ))
            {
                lua_pushboolean(L, false);
                return 1;
            }
            lua_pop(L, 2);
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

bool lua_matrix_newempty(lua_State* const L, int m, int n)
{
    std::string meta("meta:");
    meta += Naming::Type_Matrix;
    meta += "#";

    switch(lua_type(L, -1))
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
            if(luaL_getmetafield(L, 4, "__type"))
            {
                int type = lua_tointeger(L, -1);
                meta += ParameterTypeSystem::getParameterName(type);
                lua_pop(L, 1);
            }
            break;
        }
    default:
        {
            LoggingSystem::Error("Matrix type is not supported");
            return false;
        }
    }

    if(luaL_getmetatable(L, meta.c_str()))
    {
        if(lua_getfield(L, -1, "__new") == LUA_TFUNCTION)
        {
            lua_remove(L, -2);
            lua_pushinteger(L, m);
            lua_pushinteger(L, n);
            lua_call(L, 2, 1);
            return true;
        }
    }
    return false;
}
/*
void DUMP_STACK(lua_State* const L)
{
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        std::cout << i << ": " << luaL_tolstring(L, i, NULL) << std::endl;
        lua_pop(L, 1);
    }
    std::cout << "#######################" << std::endl;
}
*/
bool lua_matrix_arith1(lua_State* const L, int op)
{
    struct T_MatrixDef* m1 = *((struct T_MatrixDef**)lua_touserdata(L, 1));
    lua_pop(L, 1);

    for(int i = 0; i < m1->rows; i++)
    {
        for(int j = 0; j < m1->cols; j++)
        {
            luaL_getmetafield(L, 1, "__get");
            lua_pushvalue(L, 1);
            lua_pushinteger(L, i);
            lua_pushinteger(L, j);
            lua_pcall(L, 3, 1, 0);
            lua_arith(L, op);

            if(!(i || j)) // i == 0 && j == 0
            {
                if(lua_matrix_newempty(L, m1->rows, m1->cols))
                {
                    lua_insert(L, 2);
                }
                else
                {
                    LoggingSystem::Error("result matrix could not be created");
                    return false;
                }
            }

            if(luaL_getmetafield(L, 2, "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 2);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_rotate(L, -5, -1);
                lua_pcall(L, 4, 0, 0);
            }
            else
            {
                LoggingSystem::Error("result matrix has no set function");
                return false;
            }
        }
    }
    return true;
}

bool lua_matrix_arith2(lua_State* const L, int op)
{
    bool arg1 = false;
    bool arg2 = false;
    int r, c;

    if(luaL_getmetafield(L, 1, "__type"))
    {
        if(ParameterTypeSystem::getParameterBase(lua_tointeger(L, -1)) != (unsigned)ParameterTypeSystem::pid_matrix)
        {
            arg1 = true;
        }
        lua_pop(L, 1);
    }
    else
    {
        arg1 = true;
    }

    if(luaL_getmetafield(L, 2, "__type"))
    {
        if(ParameterTypeSystem::getParameterBase(lua_tointeger(L, -1)) != (unsigned)ParameterTypeSystem::pid_matrix)
        {
            arg2 = true;
        }
        lua_pop(L, 1);
    }
    else
    {
        arg2 = true;
    }

    if(arg1 && arg2)
    {
        LoggingSystem::Error("no valid matrix supplied");
        return false;
    }

    struct T_MatrixDef* m1 = nullptr;
    if(!arg1)
    {
        m1 = *((struct T_MatrixDef**)lua_touserdata(L, 1));
        r = m1->rows;
        c = m1->cols;
    }

    struct T_MatrixDef* m2 = nullptr;
    if(!arg2)
    {
        m2 = *((struct T_MatrixDef**)lua_touserdata(L, 2));
        r = m2->rows;
        c = m2->cols;
    }

    if((!arg1 && !arg2) && (m1->rows != m2->rows || m1->cols != m2->cols))
    {
        return false;
    }

    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            if(arg1)
            {
                lua_pushvalue(L, 1);
            }
            else
            {
                luaL_getmetafield(L, 1, "__get");
                lua_pushvalue(L, 1);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pcall(L, 3, 1, 0);
            }

            if(arg2)
            {
                lua_pushvalue(L, 2);
            }
            else
            {
                luaL_getmetafield(L, 2, "__get");
                lua_pushvalue(L, 2);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pcall(L, 3, 1, 0);
            }

            lua_arith(L, op);

            if(!(i || j)) // i == 0 && j == 0
            {
                if(lua_matrix_newempty(L, r, c))
                {
                    lua_insert(L, 3);
                }
                else
                {
                    LoggingSystem::Error("result matrix could not be created");
                    return false;
                }
            }

            if(luaL_getmetafield(L, 3, "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 3);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_rotate(L, -5, -1);
                lua_pcall(L, 4, 0, 0);
            }
            else
            {
                LoggingSystem::Error("result matrix has no set function");
                return false;
            }
        }
    }
    return true;
}

int lua_matrix_add(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPADD))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_sub(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPSUB))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_mul(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPMUL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_div(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPDIV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_mod(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPMOD))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_pow(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPPOW))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_unm(lua_State* const L)
{
    if(lua_matrix_arith1(L, LUA_OPUNM))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_idiv(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPIDIV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_band(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPBAND))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_bor(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPBOR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_bxor(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPBXOR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_bnot(lua_State* const L)
{
    if(lua_matrix_arith1(L, LUA_OPBNOT))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_shl(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPSHL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_shr(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPSHR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lua_matrix_get(lua_State* const L)
{
    if(lua_gettop(L) == 2)
    {
        struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, lua_upvalueindex(1)));
        int isnum;
        int i = lua_tointegerx(L, 1, &isnum);
        if(!isnum)
        {
            LoggingSystem::Error("argument `1` is not a valid index.");
        }
        int j = lua_tointegerx(L, 2, &isnum);
        if(!isnum)
        {
            LoggingSystem::Error("argument `2` is not a valid index.");
        }
        if(i >= 0 && i < m->rows && j >= 0 && j < m->cols)
        {
            if(luaL_getmetafield(L, lua_upvalueindex(1), "__get") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
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
    }
    return 0;
}

int lua_matrix_set(lua_State* const L)
{
    if(lua_gettop(L) == 3)
    {
        struct T_MatrixDef* m = *((struct T_MatrixDef **)lua_touserdata(L, lua_upvalueindex(1)));
        int isnum;
        int i = lua_tointegerx(L, 1, &isnum);
        if(!isnum)
        {
            LoggingSystem::Error("argument `1` is not a valid index.");
        }
        int j = lua_tointegerx(L, 2, &isnum);
        if(!isnum)
        {
            LoggingSystem::Error("argument `2` is not a valid index.");
        }
        if(luaL_getmetafield(L, lua_upvalueindex(1), "__type"))
        {
            int type = lua_tointeger(L, -1);
            if(ParameterTypeSystem::getParameterType(3) != ParameterTypeSystem::getParameterTag(type))
            {
                LoggingSystem::Error("Invalid type argument `3`.");
                return 0;
            }
        }
        else
        {
            return 0;
        }
        if(i >= 0 && i < m->rows && j >= 0 && j < m->cols)
        {
            if(luaL_getmetafield(L, lua_upvalueindex(1), "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pushvalue(L, 3);
                lua_pcall(L, 4, 0, 0);
                return 1;
            }
            else
            {
                LoggingSystem::Error("no set method defined");
            }
        }
        else
        {
            LoggingSystem::Error("Index out of bounds in matrix get");
        }
    }
    return 0;
}

int lua_matrix_transpose(lua_State* const L)
{

}

int lua_matrix_all(lua_State* const L)
{

}

int lua_matrix_any(lua_State* const L)
{

}

int lua_matrix_foreach(lua_State* const L)
{

}

int lua_matrix_copy(lua_State* const L)
{

}
