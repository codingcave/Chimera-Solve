#ifndef PARAMETERTYPE_H
#define PARAMETERTYPE_H

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace chimera {
    //typedef void (*fn_initialize)(lua_State* const L, const int& type);
    //typedef void (*fn_pushValue)(lua_State* const L, void * const value, const int& type);
    //typedef void (*fn_deleteValue)(lua_State* const L, void * const value, const int& type);

    struct T_ParameterDef
    {
        lua_CFunction initialize;
        lua_CFunction pushValue;
        lua_CFunction deleteValue;
        lua_CFunction dispose;
    };

    struct T_Parameter
    {
        size_t const type;
        void * const value;
    };

    class ParameterType final
    {
        friend class ParameterTypeSystem;
        public:
            virtual ~ParameterType();
            void initialize(lua_State * const L) const;
            void initialize(lua_State * const L, const int& id) const;
            void pushValue(lua_State * const L, void * const value) const;
            void deleteValue(lua_State* const L, void * const value) const;
            bool castFrom(lua_State* const L, size_t type, void * const value) const;

            int getID() const;
            const std::string getName() const;
            size_t getBase() const;
            size_t getTag() const;
            size_t getFlag(const std::string& flag) const;
            void addCast(size_t from, lua_CFunction fn_cast);
        protected:

        private:
            ParameterType();
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef);
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag);
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& flags);
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags);
            static size_t getType(lua_State* const L, const int& index);
            void dispose(lua_State* const L) const;
            void const * const getOrigin() const;
            const int _id;
            const std::string _name;
            void const * const _origin;
            const lua_CFunction _initialize;
            const lua_CFunction _pushValue;
            const lua_CFunction _deleteValue;
            const lua_CFunction _dispose;
            const size_t _base;
            const size_t _tag;
            std::unordered_map<size_t, lua_CFunction> _castFuntions;
            std::unordered_map<std::string, size_t> _flags;
    };
}

#endif // PARAMETERTYPE_H
