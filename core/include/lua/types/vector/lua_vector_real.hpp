#ifndef LUA_VECTOR_REAL_H
#define LUA_VECTOR_REAL_H

extern const std::string vectorRealMetaName;

void luat_vector_real_init(lua_State* const L, const int& type);
void luat_vector_real_delete(lua_State* const L, void * const value, const int& type);
int lua_vector_real_new(lua_State* const L);
int lua_vector_real_get(lua_State* const L);
int lua_vector_real_set(lua_State* const L);
int lua_vector_real_norm(lua_State* const L);

#endif // LUA_VECTOR_REAL_H
