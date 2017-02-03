#ifndef LUA_VECTOR_COMPLEX_H
#define LUA_VECTOR_COMPLEX_H

extern const std::string vectorComplexMetaName;

void luat_vector_complex_init(lua_State* const L, const int& type);
void luat_vector_complex_delete(lua_State* const L, void * const value, const int& type);
int lua_vector_complex_new(lua_State* const L);
int lua_vector_complex_get(lua_State* const L);
int lua_vector_complex_set(lua_State* const L);
int lua_vector_complex_norm(lua_State* const L);

#endif // LUA_VECTOR_COMPLEX_H
