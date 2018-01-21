#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <complex>
#include <cmath>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
//#include "RuntimeNames.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"

#include "lua/types/lua_vector.hpp"
#include "lua/types/vector/lua_vector_real.hpp"
#include "lua/types/vector/lua_vector_vector_real.hpp"

namespace ublas = boost::numeric::ublas;

static size_t vector_vector_real_pid = 0;
static size_t vector_real_pid = 0;

int chimera::runtime::types::luat_vector_vector_real_init(lua_State* const L)
{
    vector_vector_real_pid = lua_tointeger(L, 2);
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
    lua_rawget(L, LUA_REGISTRYINDEX);
    ChimeraRuntime* chSys = (ChimeraRuntime*)lua_touserdata(L, -1);
    lua_pop(L, 2);
    vector_real_pid = chSys->getTypeSystem()->getParameterTag(vector_vector_real_pid);

    lua_pushcfunction (L, lua_vector_vector_real_get);
    lua_setfield(L, -2, "__get");
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::luat_vector_vector_real_delete(lua_State* const L)
{
    void * const value = lua_touserdata(L, 2);
    const struct chimera::simulation::T_VectorDef* vd = (const struct chimera::simulation::T_VectorDef*)value;
    if(vd->deleteValue)
    {
        vec_vec_real* v = (vec_vec_real*)vd->value;
        delete v;
    }
    delete vd;

    return 0;
}

int chimera::runtime::types::lua_vector_vector_real_get(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    vec_vec_real* v = (vec_vec_real*)vd->value;

    struct chimera::simulation::T_VectorDef** vd2 = (struct chimera::simulation::T_VectorDef**)lua_newuserdata(L, sizeof(struct chimera::simulation::T_VectorDef*));
    int s = ((*v)(a)).size();
    auto vvi = new ublas::vector<double>((*v)(a));
    *vd2 = new struct chimera::simulation::T_VectorDef({s, vd->readonly, false, vvi});

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, vector_real_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}