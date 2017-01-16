#ifndef LUA_SIMULATION_H
#define LUA_SIMULATION_H

void luat_Simulation_init(lua_State* const L, const int& type);
void luat_Simulation_delete(lua_State* const L, void * const value, const int& type);
int lua_Simulation_tostring(lua_State* L);
int lua_Simulation_index(lua_State* L);
int lua_Simulation_start(lua_State* L);
int lua_Simulation_stop(lua_State* L);
int lua_Simulation_step(lua_State* L);
int lua_Simulation_run(lua_State* L);

#endif // LUA_SIMULATION_H
