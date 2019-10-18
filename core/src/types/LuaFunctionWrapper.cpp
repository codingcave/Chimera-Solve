#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "types/lua_basetypes.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"

chimera::LuaFunctionWrapper::LuaFunctionWrapper(const LuaFunctionWrapper& wrapper):
    _chimeraSystem(wrapper._chimeraSystem),
    _fn(wrapper._fn),
    _origin(wrapper._origin)
{
    //ctor
}

chimera::LuaFunctionWrapper::LuaFunctionWrapper(ChimeraSystem* chimeraSystem):
    _chimeraSystem(chimeraSystem),
    _fn(nullptr),
    _origin(this)
{
    // ctor
}

chimera::LuaFunctionWrapper::LuaFunctionWrapper(ChimeraSystem* chimeraSystem, fn_luafnwrapper fn):
    _chimeraSystem(chimeraSystem),
    _fn(fn),
    _origin((void*)fn)
{
    //ctor
}

chimera::LuaFunctionWrapper::~LuaFunctionWrapper()
{
    // Delete function from global functions
    //dtor
}

bool chimera::LuaFunctionWrapper::intern() const
{
    return _fn != nullptr;
}

void const * const chimera::LuaFunctionWrapper::getOrigin() const
{
    return _origin;
}

chimera::vec_t_LuaItem chimera::LuaFunctionWrapper::operator()(vec_t_LuaItem& params) const
{
    if(intern())
    {
        return (*_fn)(params);
    }
    else
    {
        if(_chimeraSystem) {
            lua_State* L = _chimeraSystem->getLuaState();
            if(lua_checkstack(L, params.size() + 1)) {
                int pos1 = lua_gettop(L);
                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_pushlightuserdata(L, _origin);
                lua_rawget(L, -2);
                lua_remove(L, -2);
                for(auto it = params.begin(); it != params.end(); it++)
                    _chimeraSystem->getTypeSystem()->pushValue(L, {it->getType(), it->getValue()});
                lua_call(L, params.size(), LUA_MULTRET);
                int pos2 = lua_gettop(L);
                vec_t_LuaItem result;
                for(int i = pos1 + 1; i <= pos2; i++)
                {
                    result.push_back(_chimeraSystem->getTypeSystem()->getValue(L, i));
                }

                if(pos2 > pos1)
                {
                    lua_pop(L, pos2 - pos1);
                }
                return result;
            } else {
                //LoggingSystem::Error("cannot allocate enough memory for that many parameters.");
                return vec_t_LuaItem();
            }
        } else {
            //LoggingSystem::Error("ParameterSystem is not associated with a Lua instance");
            return vec_t_LuaItem();
        }
    }
}
