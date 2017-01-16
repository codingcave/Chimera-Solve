#ifndef LUA_MATRIX_COMMON_H
#define LUA_MATRIX_COMMON_H

void push_matrix_library(lua_State* const L);
int lua_matrix_new(lua_State* const L);
int lua_matrix_eye(lua_State* const L);
int lua_matrix_ones(lua_State* const L);
int lua_matrix_zeros(lua_State* const L);

#endif // LUA_MATRIX_COMMON_H
