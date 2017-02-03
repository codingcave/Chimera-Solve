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
#include "lua/types/vector/lua_vector_vector_real.hpp"

namespace ublas = boost::numeric::ublas;

const std::string vectorVectorRealMetaName = (std::string("meta:") + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));

void luat_vector_vector_real_init(lua_State* const L, const int& type)
{
    lua_pushcfunction (L, lua_vector_vector_real_get);
    lua_setfield(L, -2, "__get");
}

void luat_vector_vector_real_delete(lua_State* const L, void * const value, const int& type)
{
    const struct T_VectorDef* vd = (const struct T_VectorDef*)value;
    if(vd->deleteValue) {
        vec_vec_real* v = (vec_vec_real*)vd->value;
        delete v;
    }
    delete vd;
}

int lua_vector_vector_real_get(lua_State* const L)
{
    struct T_VectorDef* vd = *((struct T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    vec_vec_real* v = (vec_vec_real*)vd->value;

    struct T_VectorDef** vd2 = (struct T_VectorDef**)lua_newuserdata(L, sizeof(struct T_VectorDef*));
    int s = ((*v)(a)).size();
    auto vva = (*v)(a);
    auto vvi = new ublas::vector<double>((*v)(a));
    *vd2 = new struct T_VectorDef({s, vd->readonly, false, vvi});
    luaL_setmetatable(L, vectorVectorRealMetaName.c_str());

    return 1;
}
