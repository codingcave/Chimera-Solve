#ifndef LUA_REGISTRY_H
#define LUA_REGISTRY_H

int lua_Registry_call(lua_State* L);

int lua_Registry_tostring(lua_State* L);

int lua_Instance_tostring(lua_State* L);

int lua_Instance_destroy(lua_State* L);

int lua_Instance_index(lua_State* L);

void instance_pushValue(lua_State* const L, void * const value, const int& type);
void* instance_getValue(lua_State* const L, const int& index, const int& type);
void instance_deleteValue(lua_State* const L, void * const value, const int& type);

#endif // LUA_REGISTRY_H
