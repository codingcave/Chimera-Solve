#include <math.h>
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
#include "lua/lua_static.hpp"
#include "lua/types/lua_complex.hpp"

#include "lua/lua_math.hpp"

void push_math_library(lua_State* const L)
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
    lua_pushstring(L, "__index");
    lua_newtable(L);
    lua_pushcfunction (L, lua_math_abs);
    lua_setfield(L, -2, "abs");
    lua_pushcfunction (L, lua_math_exp);
    lua_setfield(L, -2, "exp");
    lua_pushcfunction (L, lua_math_log);
    lua_setfield(L, -2, "log");
    lua_pushcfunction (L, lua_math_log10);
    lua_setfield(L, -2, "log10");
    lua_pushcfunction (L, lua_math_pow);
    lua_setfield(L, -2, "pow");
    lua_pushcfunction (L, lua_math_sqrt);
    lua_setfield(L, -2, "sqrt");
    lua_pushcfunction (L, lua_math_sin);
    lua_setfield(L, -2, "sin");
    lua_pushcfunction (L, lua_math_cos);
    lua_setfield(L, -2, "cos");
    lua_pushcfunction (L, lua_math_tan);
    lua_setfield(L, -2, "tan");
    lua_pushcfunction (L, lua_math_asin);
    lua_setfield(L, -2, "asin");
    lua_pushcfunction (L, lua_math_acos);
    lua_setfield(L, -2, "acos");
    lua_pushcfunction (L, lua_math_atan);
    lua_setfield(L, -2, "atan");
    lua_pushcfunction (L, lua_math_sinh);
    lua_setfield(L, -2, "sinh");
    lua_pushcfunction (L, lua_math_cosh);
    lua_setfield(L, -2, "cosh");
    lua_pushcfunction (L, lua_math_tanh);
    lua_setfield(L, -2, "tanh");
    lua_pushcfunction (L, lua_math_asinh);
    lua_setfield(L, -2, "asinh");
    lua_pushcfunction (L, lua_math_acosh);
    lua_setfield(L, -2, "acosh");
    lua_pushcfunction (L, lua_math_atanh);
    lua_setfield(L, -2, "atanh");
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);
}

int lua_math_abs(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, abs(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                lua_pushnumber(L, std::abs(**a));
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.abs is not a number");
    return 0;
}

int lua_math_exp(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, exp(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::exp(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.exp is not a number");
    return 0;
}

int lua_math_log(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, log(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::log(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.log is not a number");
    return 0;
}

int lua_math_log10(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, log10(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::log10(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.log10 is not a number");
    return 0;
}

int lua_math_pow(lua_State* const L)
{
    if(lua_gettop(L) == 2)
    {
        if(lua_isnumber(L, 1) && lua_isnumber(L, 2))
        {
            lua_pushnumber(L, pow(lua_tonumber(L, 1), lua_tonumber(L, 2)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            std::complex<double> **b = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if((a || lua_isnumber(L, 1)) && (b || lua_isnumber(L, 2)))
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                luaL_setmetatable(L, complexMetaName.c_str());
                if(a)
                {
                    if(b)
                    {
                        *c = new std::complex<double>(std::pow(**a, **b));
                    }
                    else
                    {
                        *c = new std::complex<double>(std::pow(**a, lua_tonumber(L, 2)));
                    }
                }
                else
                {
                    *c = new std::complex<double>(std::pow(lua_tonumber(L, 1), **b));
                }

                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.pow is not a number");
    return 0;
}

int lua_math_sqrt(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, sqrt(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::sqrt(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.sqrt is not a number");
    return 0;
}

int lua_math_sin(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, sin(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::sin(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.sin is not a number");
    return 0;
}

int lua_math_cos(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, cos(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::cos(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.cos is not a number");
    return 0;
}

int lua_math_tan(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, tan(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::tan(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.tan is not a number");
    return 0;
}

int lua_math_asin(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, asin(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::asin(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.asin is not a number");
    return 0;
}

int lua_math_acos(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, acos(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::acos(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.acos is not a number");
    return 0;
}

int lua_math_atan(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, atan(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::atan(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.atan is not a number");
    return 0;
}

int lua_math_sinh(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, sinh(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::sinh(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.sinh is not a number");
    return 0;
}

int lua_math_cosh(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, cosh(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::cosh(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.cosh is not a number");
    return 0;
}

int lua_math_tanh(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, tanh(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::tanh(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.tanh is not a number");
    return 0;
}

int lua_math_asinh(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, asinh(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::asinh(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.asinh is not a number");
    return 0;
}

int lua_math_acosh(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, acosh(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::acosh(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.acosh is not a number");
    return 0;
}

int lua_math_atanh(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        if(lua_isnumber(L, 1))
        {
            lua_pushnumber(L, atanh(lua_tonumber(L, 1)));
            return 1;
        }
        else
        {
            std::complex<double> **a = (std::complex<double> **)luaL_checkudata(L, 1, complexMetaName.c_str());
            if(a)
            {
                std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
                *c = new std::complex<double>(std::atanh(**a));
                luaL_setmetatable(L, complexMetaName.c_str());
                return 1;
            }
        }
    }
    LoggingSystem::Error("argument `1` to math.atanh is not a number");
    return 0;
}
