#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "Naming.hpp"
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
    return _fn == nullptr;
}

struct T_Parameter LuaFunctionWrapper::operator()(vec_t_LuaItem& params) const
{
    if(intern())
    {
        struct T_Parameter result = (*_fn)(params);
        return result;
    }
    else
    {
        if(ParameterTypeSystem::_instance._luaparser) {
            lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
            if(lua_checkstack(L, params.size() + 1)) {
                lua_pushstring(L, Naming::Lua_reg_functions);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_pushlightuserdata(L, (void*)this);
                lua_rawget(L, -2);
                lua_remove(L, -2);
                for(auto it = params.begin(); it != params.end(); it++)
                    ParameterTypeSystem::pushValue(*it);
                lua_call(L, params.size(), 1);

                T_Parameter p = ParameterTypeSystem::getValue(-1);
                lua_pop(L, 1);
                return p;
            } else {
                LoggingSystem::Error("cannot allocate enough memory for that many parameters.");
                return { -1, nullptr };
            }
        } else {
            LoggingSystem::Error("ParameterSystem is not associated with a Lua instance");
            return { -1, nullptr };
        }
    }
}
