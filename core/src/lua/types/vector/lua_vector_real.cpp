#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <complex>
#include <cmath>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "LoggingSystem.hpp"
#include "lua/lua_static.hpp"

#include "types/lua_vector.hpp"
#include "lua/types/vector/lua_vector_real.hpp"

namespace ublas = boost::numeric::ublas;

const std::string vectorRealMetaName = (std::string("meta:") + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));

void luat_vector_real_init(lua_State* const L, const int& type)
{
    lua_pushcfunction (L, lua_vector_real_new);
    lua_setfield(L, -2, "__new");
    lua_pushcfunction (L, lua_vector_real_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_vector_real_set);
    lua_setfield(L, -2, "__set");
    lua_getfield(L, -1, "__methods");
    map_t_LuaMethods* vectorMethods = (map_t_LuaMethods*)lua_touserdata(L, -1);
    (*vectorMethods)["norm"] = lua_vector_real_norm;
}

void luat_vector_real_delete(lua_State* const L, void * const value, const int& type)
{
    const struct T_VectorDef* vd = (const struct T_VectorDef*)value;
    if(vd->deleteValue)
    {
        const ublas::vector<double>* v = (const ublas::vector<double>*)vd->value;
        delete v;
    }
    delete vd;
}

int lua_vector_real_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    ublas::vector<double>* v = new ublas::vector<double>(a);
    if(lua_gettop(L) > 1)
    {
        double val = lua_tonumber(L, 2);
        for(int i = 0; i < a; i++)
            (*v)(i) = val;
    }
    struct T_VectorDef** vd = (struct T_VectorDef**)lua_newuserdata(L, sizeof(struct T_VectorDef*));
    *vd = new struct T_VectorDef({a, false, true, v});
    luaL_setmetatable(L, vectorRealMetaName.c_str());
    return 1;
}

int lua_vector_real_get(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    ublas::vector<double>* v = (ublas::vector<double>*)vd->value;
    lua_pushnumber(L, (*v)(a));
    return 1;
}

int lua_vector_real_set(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    double val = lua_tonumber(L, 3);
    ublas::vector<double>* v = (ublas::vector<double>*)vd->value;

    (*v)(a) = val;
    return 0;
}

int lua_vector_real_norm(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, lua_upvalueindex(1)));
    ublas::vector<double>* v = (ublas::vector<double>*)vd->value;
    int length = v->size();
    double sum = 0;
    for(int i = 0; i < length; i++)
        sum += (*v)(i) * (*v)(i);
    lua_pushnumber(L, sqrt(sum));
    return 1;
}
