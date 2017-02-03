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
#include "lua/types/vector/lua_vector_complex.hpp"

namespace ublas = boost::numeric::ublas;

const std::string vectorComplexMetaName = (std::string("meta:") + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_Complex));

void luat_vector_complex_init(lua_State* const L, const int& type)
{
    lua_pushcfunction (L, lua_vector_complex_new);
    lua_setfield(L, -2, "__new");
    lua_pushcfunction (L, lua_vector_complex_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_vector_complex_set);
    lua_setfield(L, -2, "__set");
    lua_getfield(L, -1, "__methods");
    map_t_LuaMethods* vectorMethods = (map_t_LuaMethods*)lua_touserdata(L, -1);
    (*vectorMethods)["norm"] = lua_vector_complex_norm;
}

void luat_vector_complex_delete(lua_State* const L, void * const value, const int& type)
{
    const struct T_VectorDef* vd = (const struct T_VectorDef*)value;
    if(vd->deleteValue)
    {
        const ublas::vector<std::complex<double> >* v = (const ublas::vector<std::complex<double> >*)vd->value;
        delete v;
    }
    delete vd;
}

int lua_vector_complex_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    ublas::vector<std::complex<double> >* v = new ublas::vector<std::complex<double> >(a);
    if(lua_gettop(L) > 1)
    {
        std::complex<double>* val = *((std::complex<double>**)lua_touserdata(L, 2));
        for(int i = 0; i < a; i++)
            (*v)(i) = std::complex<double>(*val);
    }
    struct T_VectorDef** vd = (struct T_VectorDef**)lua_newuserdata(L, sizeof(struct T_VectorDef*));
    *vd = new struct T_VectorDef({a, false, true, v});
    luaL_setmetatable(L, vectorComplexMetaName.c_str());
    return 1;
}

int lua_vector_complex_get(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    ublas::vector<std::complex<double> >* v = (ublas::vector<std::complex<double> >*)vd->value;
    std::complex<double>* val = new std::complex<double>((*v)(a));
    ParameterTypeSystem::pushValue({ParameterTypeSystem::getParameterID(Naming::Type_Complex), val});
    return 1;
}

int lua_vector_complex_set(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, 1));
    ublas::vector<std::complex<double> >* v = (ublas::vector<std::complex<double> >*)vd->value;
    int a = lua_tointeger(L, 2);

    if(lua_isnumber(L, 3))
    {
        double val = lua_tonumber(L, 3);
        (*v)(a) = val;
        return 0;
    }

    if(luaL_getmetafield(L, 3, "__type") && lua_tointeger(L, 3) == ParameterTypeSystem::getParameterID(Naming::Type_Complex))
    {
        std::complex<double>* val = *((std::complex<double>**)lua_touserdata(L, 3));

        (*v)(a) = std::complex<double>(*val);
        return 0;
    }

    return 0;
}

int lua_vector_complex_norm(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, lua_upvalueindex(1)));
    ublas::vector<std::complex<double> >* v = (ublas::vector<std::complex<double> >*)vd->value;
    int length = v->size();
    double sum = 0;
    double vi = 0;
    for(int i = 0; i < length; i++)
    {
        vi = std::abs((*v)(i));
        sum += vi * vi;
    }

    lua_pushnumber(L, sqrt(sum));
    return 1;
}
