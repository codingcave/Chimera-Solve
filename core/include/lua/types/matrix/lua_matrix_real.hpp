#ifndef LUA_MATRIX_REAL_H
#define LUA_MATRIX_REAL_H

extern const std::string matrixRealMetaName;

void luat_matrix_real_init(lua_State* const L, const int& type);
void luat_matrix_real_delete(lua_State* const L, void * const value, const int& type);
int lua_matrix_real_new(lua_State* const L);
int lua_matrix_real_get(lua_State* const L);
int lua_matrix_real_set(lua_State* const L);
int lua_matrix_real_det(lua_State* const L);
int lua_matrix_real_inv(lua_State* const L);
int lua_matrix_real_dot(lua_State* const L);
int lua_matrix_real_trace(lua_State* const L);
int lua_matrix_real_eigen(lua_State* const L);
int lua_matrix_real_solve(lua_State* const L);

#endif // LUA_MATRIX_REAL_H
