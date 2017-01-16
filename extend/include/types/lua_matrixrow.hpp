#ifndef LUA_MATRIXROW_H
#define LUA_MATRIXROW_H

void luat_matrixrow_init(lua_State* const L, const int& type);
void luat_matrixrow_delete(lua_State* const L, void * const value, const int& type);
int lua_matrixrow_newindex(lua_State* const L);
int lua_matrixrow_iterator(lua_State* const L);
int lua_matrixrow_len(lua_State* const L);
int lua_matrixrow_tostring(lua_State* const L);
int lua_matrixrow_index(lua_State* const L);

#endif // LUA_MATRIXROW_H
