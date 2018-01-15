#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "types/lua_static.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "types/lua_Module.hpp"
#include "ChimeraSystem.hpp"

/*

void chimera::ChimeraSystem::notifyUnload(const std::string& name, EntryPoint *ep, Module * const reg)
{
    //reg->truncate();
    // delete all instances??
    /*
    lua_pushstring(_L, "simulation:system_names");
    lua_gettable(_L, LUA_REGISTRYINDEX);
    lua_pushstring(_L, ep->getName().c_str());
    lua_rawget(_L, -2);
    luaL_getmetafield(_L, -1, "__loaded");
    lua_pushlstring(_L, name.c_str(), name.size());
    lua_pushnil(_L);
    lua_rawset(_L, -3);
    lua_pop(_L, 3);
    _loader->Unload(name);
    * /
}

*/

void chimera::ChimeraSystem::pushModule(EntryPoint* ep, const std::string& name, Module const * const module)
{
    int top = lua_gettop(_L);
    std::string instanceTypeName("$");
    instanceTypeName += name;

    lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_rawget(_L, LUA_REGISTRYINDEX);
    if(lua_rawgetp(_L, -1, ep))
    {
        luaL_getmetafield(_L, -1, "__loaded");

        int instType = _typeSys->registerParameter(instanceTypeName, module, {nullptr, instance_pushValue, instance_deleteValue, nullptr}, chimera::systemtypes::PID_INSTANCE, (size_t)ep);

        const Module **lmod = (const Module **)lua_newuserdata(_L, sizeof(Module *));
        *lmod = module;
        int modPos = lua_absindex(_L, -1);
        // module metatable
        lua_newtable (_L);
        lua_pushcfunction (_L, chimera::lua_ignore_newindex);
        lua_setfield(_L, -2, "__newindex");
        lua_pushcfunction (_L, chimera::lua_Module_call);
        lua_setfield(_L, -2, "__call");
        lua_pushcfunction (_L, chimera::lua_Module_tostring);
        lua_setfield(_L, -2, "__tostring");
        lua_pushstring(_L, chimera::luanames::LUA_NAME_MODULE);
        lua_setfield(_L, -2, "__name");
        lua_pushinteger(_L, chimera::systemtypes::PID_MODULE);
        lua_setfield(_L, -2, "__type");
        lua_pushlightuserdata(_L, ep);
        lua_setfield(_L, -2, "__epptr");
        lua_pushlstring(_L, name.c_str(), name.size());
        lua_setfield(_L, -2, "__module");
        lua_pushinteger(_L, instType);
        lua_setfield(_L, -2, "__itype");
        lua_pushlstring(_L, "__index", 7);
        lua_newtable(_L);
        for(auto it = module->beginItems(); it != module->endItems(); it++)
        {
            lua_pushlstring(_L, it->first.c_str(), it->first.size());
            pushLuaValue(_L, it->second.getType(), it->second.getValue());
            lua_rawset(_L, -3);
        }

        lua_rawset(_L, -3);
        lua_setmetatable(_L, -2);
        lua_pushlightuserdata(_L, (void*)module);
        lua_pushvalue(_L, -2);
        lua_rawset(_L, -6);

        lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_rawget(_L, LUA_REGISTRYINDEX);
        lua_rawgeti(_L, -1, instType);

        lua_pushcfunction (_L, chimera::lua_Instance_tostring);
        lua_setfield(_L, -2, "__tostring");
        lua_pushcfunction (_L, chimera::lua_ignore_newindex);
        lua_setfield(_L, -2, "__newindex");
        lua_pushcfunction (_L, chimera::lua_Instance_index);
        lua_setfield(_L, -2, "__index");
        lua_pushstring(_L, chimera::luanames::LUA_NAME_INSTANCE);
        lua_setfield(_L, -2, "__name");
        lua_pushlightuserdata(_L, ep);
        lua_setfield(_L, -2, "__epptr");
        lua_pushlightuserdata(_L, (void*)module);
        lua_setfield(_L, -2, "__modptr");
        lua_pushlstring(_L, name.c_str(), name.size());
        lua_setfield(_L, -2, "__module");
        lua_pushvalue(_L, modPos);
        lua_pushcclosure(_L, chimera::lua_Instance_destroy, 1);
        lua_setfield(_L, -2, "__gc");
        lua_pushlstring(_L, name.c_str(), name.size());
        lua_pushvalue(_L, modPos);
        lua_rawset(_L, top + 3);
    }

    lua_settop(_L, top);
}

void chimera::ChimeraSystem::popModule(EntryPoint* ep, Module const * const module)
{
return;
    const std::string name = ep->findModule(module);
    if(name.size() > 0)
    {
        int top = lua_gettop(_L);
        lua_pushstring(_L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
        lua_rawget(_L, LUA_REGISTRYINDEX);
        if(lua_rawgetp(_L, -1, ep))
        {
            if(luaL_getmetafield(_L, -1, "__loaded"))
            {
                _typeSys->removeParameter(module);

                lua_pushlstring(_L, name.c_str(), name.size());
                int utype = lua_rawget(_L, -2);
                if(utype) {
                    chimera::Module **lmod = (chimera::Module **)lua_touserdata(_L, -1);
                    *lmod = nullptr;

                    lua_pushlstring(_L, name.c_str(), name.size());
                    lua_pushnil(_L);
                    lua_rawset(_L, -4);
                }
            }
        }

        lua_pushlightuserdata(_L, (void*)module);
        lua_pushnil(_L);
        lua_rawset(_L, top + 1);
        lua_settop(_L, top);
    }
}

int chimera::lua_Module_call(lua_State* L)
{
    const Module *mod = *((const Module **)lua_touserdata(L, 1));
    luaL_getmetafield(L, 1, "__itype");

    int type = lua_tointeger(L, -1);
    lua_pop(L, 1);
    vec_t_LuaItem items;

    for(int i = 2; i <= lua_gettop(L); i++)
    {
        items.push_back(mod->getChimeraSystem()->getTypeSystem()->getValue(L, i));
    }

    void* value = mod->getInstance(items);
    if(value)
    {
        mod->getChimeraSystem()->getTypeSystem()->pushValue(L, {type, value});
        return 1;
    }
    return 0;
}

int chimera::lua_Module_tostring(lua_State* L)
{
    const Module *lmod = *((const Module **)lua_touserdata(L, 1));

    std::string txt(chimera::luanames::LUA_NAME_MODULE);
    txt += "<";
    if(lmod)
    {
        luaL_getmetafield(L, 1, "__module");
        txt += lua_tostring(L, -1);
    }
    else
    {
        txt += "RefError";
    }
    txt += ">";

    lua_pushstring(L, txt.c_str());
    return 1;
}

int chimera::lua_Instance_tostring(lua_State* L)
{
    const void *inst = *((const void **)lua_touserdata(L, 1));

    std::string txt(chimera::luanames::LUA_NAME_INSTANCE);
    txt += "<";
    if(inst)
    {
        lua_getmetatable(L, -1);
        lua_getfield(L, -1, "__module");
        txt += lua_tostring(L, -1);
    }
    else
    {
        txt += "RefError";
    }
    txt += ">";

    lua_pushstring(L, txt.c_str());
    return 1;
}

int chimera::lua_Instance_destroy(lua_State* L)
{
    void* item = *((void**)lua_touserdata(L, 1));
    const Module *mod = *((const Module **)lua_touserdata(L, lua_upvalueindex(1)));
    if(mod != nullptr && item != nullptr)
    {
        mod->destroyInstance(item);
    }
    return 0;
}

int chimera::lua_Instance_forwardMethod(lua_State* L)
{
    luaL_getmetafield(L, lua_upvalueindex(1), "__modptr");
    const Module* mod = (const Module *)lua_touserdata(L, -1);
    void* instance = *((void**)lua_touserdata(L, lua_upvalueindex(1)));
    fn_instancefnwrapper method = (fn_instancefnwrapper)lua_touserdata(L, lua_upvalueindex(2));
    lua_pop(L, 1);

    vec_t_LuaItem params;
    for(int i = 1; i <= lua_gettop(L); ++i)
    {
        params.push_back(mod->getChimeraSystem()->getTypeSystem()->getValue(L, i));
    }
    lua_settop(L, 0);

    vec_t_LuaItem result = (*method)(mod, instance, params);

    for(auto it = result.begin(); it != result.end(); it++)
    {
        mod->getChimeraSystem()->getTypeSystem()->pushValue(L, {it->getType(), it->getValue()});
    }

    return result.size();
}

int chimera::lua_Instance_index(lua_State* L)
{
    luaL_getmetafield(L, 1, "__modptr");
    const Module* mod = (const Module *)lua_touserdata(L, -1);
    //int regInd = lua_absindex(L, -1);
    std::string item(lua_tostring(L, 2));

    map_t_InstanceMethods::const_iterator available = mod->methods()->find (item);
    if(available != mod->methods()->end())
    {
        lua_pushvalue(L, 1);
        lua_pushlightuserdata(L, (void*)available->second);
        lua_pushcclosure(L, lua_Instance_forwardMethod, 2);
        return 1;
    }
    return 0;
}

int chimera::instance_pushValue(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    const void **item = (const void **)lua_newuserdata(L, sizeof(void *));
    *item = value;
    return 1;
}

int chimera::instance_deleteValue(lua_State* const L)
{
    void *value = (void *)lua_touserdata(L, 2);
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 1);
    if(lua_rawget(L, -2))
    {
        lua_pushstring(L, "__modptr");
        lua_rawget(L, -2);
        const Module *lmod = (const Module *)lua_touserdata(L, -1);
        lmod->destroyInstance(value);
    }
    return 0;
}
