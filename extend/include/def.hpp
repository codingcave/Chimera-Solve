#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

typedef std::unordered_map<std::string, size_t> map_t_size;

#endif // COMMON_DEFINITIONS_H

#ifdef PARAMETERTYPE_H
#ifndef COMMON_DEFINITIONS_PARAMETERTYPE
#define COMMON_DEFINITIONS_PARAMETERTYPE
typedef std::vector<struct T_Parameter> vec_t_LuaItem;
typedef std::unordered_map<std::string, const struct T_Parameter> map_t_LuaItem;
#endif
#endif

#ifdef lua_h
#ifndef COMMON_DEFINITIONS_LUA
#define COMMON_DEFINITIONS_LUA
typedef std::unordered_map<std::string, lua_CFunction> map_t_LuaMethods;
#endif
#endif


