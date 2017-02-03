#include <iostream>
#include <vector>
#include <unordered_map>
#include "lua.hpp"

#include "Naming.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "lua/types/lua_Registry.hpp"
#include "lua/lua_static.hpp"
#include "lua/LuaParser.hpp"

void LuaParser::pushRegistry(const std::string& name, Registry const * const reg)
{
    std::string typeName(Naming::Lua_name_Registry);
    typeName += "/";
    typeName += name;
    std::string entryPointType(Naming::Lua_name_EntryPoint);
    entryPointType += "/";
    entryPointType += reg->getEntrypoint();

    int type = ParameterTypeSystem::registerParameter(typeName, {nullptr, nullptr, nullptr}, ParameterTypeSystem::getParameterID(Naming::Type_Registry), ParameterTypeSystem::getParameterID(entryPointType), false);

    const Registry **lreg = (const Registry **)lua_newuserdata(_L, sizeof(Registry *));
    int regPos = lua_absindex(_L, -1);
    *lreg = reg;

    // registry metatable
    lua_newtable (_L);
    lua_pushcfunction (_L, lua_ignore_newindex);
    lua_setfield(_L, -2, "__newindex");
    lua_pushcfunction (_L, lua_Registry_call);
    lua_setfield(_L, -2, "__call");
    lua_pushcfunction (_L, lua_Registry_tostring);
    lua_setfield(_L, -2, "__tostring");
    lua_pushstring(_L, Naming::Lua_name_Registry);
    lua_setfield(_L, -2, "__name");
    lua_pushinteger(_L, type);
    lua_setfield(_L, -2, "__type");
    lua_pushlstring(_L, reg->getEntrypoint().c_str(), reg->getEntrypoint().size());
    lua_setfield(_L, -2, "__ep");
    lua_pushlstring(_L, name.c_str(), name.size());
    lua_setfield(_L, -2, "__reg");

    lua_pushlstring(_L, "__index", 7);
    lua_newtable(_L);
    for(auto it = reg->beginItems(); it != reg->endItems(); it++)
    {
        lua_pushlstring(_L, it->first.c_str(), it->first.size());
        pushLuaValue(it->second);
        lua_rawset(_L, -3);
    }

    lua_rawset(_L, -3);
    lua_setmetatable(_L, -2);

    lua_pushstring(_L, name.c_str());
    lua_pushstring(_L, Naming::Lua_reg_systemNames);
    lua_gettable(_L, LUA_REGISTRYINDEX);
    lua_pushstring(_L, reg->getEntrypoint().c_str());
    lua_rawget(_L, -2);

    luaL_getmetafield(_L, -1, "__loaded");
    lua_pushvalue(_L, -4);
    lua_pushvalue(_L, regPos);
    lua_rawset(_L, -3);

    std::string instanceTypeName(Naming::Lua_name_Instance);
    instanceTypeName += "/";
    instanceTypeName += reg->getEntrypoint();
    instanceTypeName += "/";
    instanceTypeName += name;

    type = ParameterTypeSystem::registerParameter(instanceTypeName, {nullptr, instance_pushValue, instance_deleteValue}, ParameterTypeSystem::getParameterID(Naming::Type_Instance), ParameterTypeSystem::getParameterID(entryPointType), true);
    std::string metaName("meta:");
    metaName += instanceTypeName;
    luaL_getmetatable (_L, metaName.c_str());

    lua_pushcfunction (_L, lua_Instance_tostring);
    lua_setfield(_L, -2, "__tostring");
    lua_pushcfunction (_L, lua_ignore_newindex);
    lua_setfield(_L, -2, "__newindex");
    lua_pushcfunction (_L, lua_Instance_index);
    lua_setfield(_L, -2, "__index");
    lua_pushstring(_L, Naming::Lua_name_Instance);
    lua_setfield(_L, -2, "__name");
    lua_pushlstring(_L, reg->getEntrypoint().c_str(), reg->getEntrypoint().size());
    lua_setfield(_L, -2, "__ep");
    lua_pushlstring(_L, name.c_str(), name.size());
    lua_setfield(_L, -2, "__reg");
    lua_pushvalue(_L, regPos);
    lua_setfield(_L, -2, "__registry");
    lua_pushvalue(_L, regPos);
    lua_pushcclosure(_L, lua_Instance_destroy, 1);
    lua_setfield(_L, -2, "__gc");

    lua_pop(_L, 7);
}

int lua_Registry_call(lua_State* L)
{
    const Registry *reg = *((const Registry **)lua_touserdata(L, 1));
    luaL_getmetafield(L, 1, "__reg");
    std::string instanceName(Naming::Lua_name_Instance);
    instanceName += "/";
    instanceName += reg->getEntrypoint();
    instanceName += "/";
    instanceName += lua_tostring(L, -1);
    int type = ParameterTypeSystem::getParameterID(instanceName);
    lua_pop(L, 1);
    vec_t_LuaItem items;

    for(int i = 2; i <= lua_gettop(L); i++)
    {
        items.push_back(ParameterTypeSystem::getValue(i));
    }

    void* value = reg->getInstance(items);
    //ParameterTypeSystem::releaseValues(items);
    if(value){
        ParameterTypeSystem::pushValue({type, value});
    } else {
        ParameterTypeSystem::pushValue({0, nullptr});
    }


    return 1;
}

int lua_Registry_tostring(lua_State* L)
{
    const Registry *lreg = *((const Registry **)lua_touserdata(L, -1));
    std::string txt(lreg->getEntrypoint());
    txt += "<";
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "__reg");
    txt += lua_tostring(L, -1);
    txt += ">";

    lua_pushstring(L, txt.c_str());
    return 1;
}

int lua_Instance_tostring(lua_State* L)
{
    std::string txt(Naming::Lua_name_Instance);
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "__ep");
    txt += "<";
    txt += lua_tostring(L, -1);
    lua_getfield(L, -2, "__reg");
    txt += "/";
    txt += lua_tostring(L, -1);
    txt += ">";
    lua_settop(L, 0);
    lua_pushstring(L, txt.c_str());
    return 1;
}

int lua_Instance_destroy(lua_State* L)
{
    void* item = *((void**)lua_touserdata(L, 1));
    const Registry *reg = *((const Registry **)lua_touserdata(L, lua_upvalueindex(1)));
    reg->destroyInstance(item);
    return 0;
}

int lua_Instance_index(lua_State* L)
{
    luaL_getmetafield(L, 1, "__registry");
    const Registry* reg = *((const Registry **)lua_touserdata(L, -1));
    //int regInd = lua_absindex(L, -1);
    std::string item(lua_tostring(L, 2));

    map_t_LuaMethods::const_iterator available = reg->methods()->find (item);
    if(available != reg->methods()->end())
    {
        lua_pushvalue(L, 1);
        lua_pushcclosure(L, available->second, 1);
        return 1;
    }
    return 0;
}

void instance_pushValue(lua_State* const L, void * const value, const int& type)
{
    const void **item = (const void **)lua_newuserdata(L, sizeof(void *));
    *item = value;
    //std::string metaName("meta:");
    //metaName += ParameterTypeSystem::getParameterName(type);
    //luaL_setmetatable (L, metaName.c_str());
}

void instance_deleteValue(lua_State* const L, void * const value, const int& type)
{
    std::string typeName = ParameterTypeSystem::getParameterName(type);
    int p1 = typeName.find('/');
    int p2 = typeName.find('/', p1 + 1);
    std::string ep = typeName.substr(p1 + 1, p2 - p1 - 2);
    std::string reg = typeName.substr(p2 + 1);

    lua_pushstring(L, Naming::Lua_reg_systemNames);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushlstring(L, ep.c_str(), ep.size());
    lua_rawget(L, -2);
    luaL_getmetafield(L, -1, "__loaded");
    lua_pushlstring(L, reg.c_str(), reg.size());
    lua_rawget(L, -2);
    const Registry *lreg = *((const Registry **)lua_touserdata(L, -1));
    lreg->destroyInstance(value);
    lua_pop(L, 5);
}
