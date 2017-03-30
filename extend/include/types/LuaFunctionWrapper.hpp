#ifndef LUAFUNCTIONWRAPPER_H
#define LUAFUNCTIONWRAPPER_H

typedef ParameterValue (*fn_luafnwrapper)(const vec_t_LuaItem& params);

class LuaFunctionWrapper
{
    friend class ParameterTypeSystem;
    public:
        LuaFunctionWrapper(fn_luafnwrapper fn);
        bool intern() const;
        virtual ~LuaFunctionWrapper();
        ParameterValue operator()(vec_t_LuaItem& params) const;
    protected:
    private:
        LuaFunctionWrapper();
        fn_luafnwrapper _fn;
};

#endif // LUAFUNCTIONWRAPPER_H
