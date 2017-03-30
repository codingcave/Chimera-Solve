#ifndef PARAMETERTYPE_H
#define PARAMETERTYPE_H

typedef struct lua_State lua_State;

typedef void (*fn_initialize)(lua_State* const L, const int& type);
typedef void (*fn_pushValue)(lua_State* const L, void * const value, const int& type);
typedef void (*fn_deleteValue)(lua_State* const L, void * const value, const int& type);

struct T_ParameterDef
{
    fn_initialize initialize;
    fn_pushValue pushValue;
    fn_deleteValue deleteValue;
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
        void initialize(lua_State * const L) const;
        void initialize(lua_State * const L, const int& id) const;
        void pushValue(lua_State * const L, void * const value) const;
        void deleteValue(lua_State* const L, void * const value) const;

        int getID() const;
        const std::string getName() const;
        size_t getBase() const;
        size_t getTag() const;
    protected:

    private:
        ParameterType();
        virtual ~ParameterType();
        ParameterType(const std::string& name, const int& id, const struct T_ParameterDef& pdef);
        ParameterType(const std::string& name, const int& id, const struct T_ParameterDef& pdef, size_t base, size_t tag);
        const int _id;
        const std::string _name;
        const fn_initialize _initialize;
        const fn_pushValue _pushValue;
        const fn_deleteValue _deleteValue;
        const size_t _base;
        const size_t _tag;
};

#endif // PARAMETERTYPE_H
