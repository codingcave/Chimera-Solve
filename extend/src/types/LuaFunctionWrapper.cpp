#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "LoggingSystem.hpp"
#include "types/LuaFunctionWrapper.hpp"

LuaFunctionWrapper::LuaFunctionWrapper()
{
    //ctor
}

LuaFunctionWrapper::LuaFunctionWrapper(fn_luafnwrapper fn):
    _fn(fn)
{
    //ctor
}

LuaFunctionWrapper::~LuaFunctionWrapper()
{
    //dtor
}

bool LuaFunctionWrapper::intern() const
{
    return _fn != nullptr;
}

vec_t_LuaItem LuaFunctionWrapper::operator()(vec_t_LuaItem& params) const
{
    if(intern())
    {
        return (*_fn)(params);
    }
    else
    {
        if(ParameterTypeSystem::_instance._luaparser) {
            lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
            if(lua_checkstack(L, params.size() + 1)) {
                int pos1 = lua_gettop(L);
                lua_pushstring(L, Naming::Lua_reg_functions);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_pushlightuserdata(L, (void*)this);
                lua_rawget(L, -2);
                lua_remove(L, -2);
                for(auto it = params.begin(); it != params.end(); it++)
                    ParameterTypeSystem::pushValue({it->getType(), it->getValue()});
                lua_call(L, params.size(), LUA_MULTRET);
                int pos2 = lua_gettop(L);
                vec_t_LuaItem result;
                for(int i = pos1 + 1; i <= pos2; i++)
                {
                    result.push_back(ParameterTypeSystem::getValue(i));
                }

                if(pos2 > pos1)
                {
                    lua_pop(L, pos2 - pos1);
                }
                return result;
            } else {
                LoggingSystem::Error("cannot allocate enough memory for that many parameters.");
                return vec_t_LuaItem();
            }
        } else {
            LoggingSystem::Error("ParameterSystem is not associated with a Lua instance");
            return vec_t_LuaItem();
        }
    }
}
