#ifndef LUA_BASETYPES_H
#define LUA_BASETYPES_H


int lua_UserData_gc(lua_State* L);
void luat_UserData_push(lua_State* const L, void * const value, const int& type);
void luat_nil_push(lua_State* const L, void * const value, const int& type);
void luat_boolean_push(lua_State* const L, void * const value, const int& type);
void luat_boolean_delete(lua_State* const L, void * const value, const int& type);
void luat_real_push(lua_State* const L, void * const value, const int& type);
void luat_real_delete(lua_State* const L, void * const value, const int& type);
void luat_table_push(lua_State* const L, void * const value, const int& type);
void luat_table_delete(lua_State* const L, void * const value, const int& type);
void luat_string_push(lua_State* const L, void * const value, const int& type);
void luat_string_delete(lua_State* const L, void * const value, const int& type);
void luat_function_push(lua_State* const L, void * const value, const int& type);
int luat_luafunction_call(lua_State* L);
void luat_luafunction_init(lua_State* const L, const int& type);
void luat_luafunction_push(lua_State* const L, void * const value, const int& type);
void luat_luafunction_delete(lua_State* const L, void * const value, const int& type);

#endif // LUA_BASETYPES_H
