#ifndef LUA_VECTOR_VECTOR_REAL_H
#define LUA_VECTOR_VECTOR_REAL_H

extern const std::string vectorVectorRealMetaName;
typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > vec_vec_real;

void luat_vector_vector_real_init(lua_State* const L, const int& type);
void luat_vector_vector_real_delete(lua_State* const L, void * const value, const int& type);
int lua_vector_vector_real_get(lua_State* const L);

#endif // LUA_VECTOR_VECTOR_REAL_H
