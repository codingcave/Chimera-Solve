#ifndef LUAFUNCTIONWRAPPER_H
#define LUAFUNCTIONWRAPPER_H

namespace chimera {
    class ChimeraSystem;
    class ParameterTypeSystem;

    class LuaFunctionWrapper
    {
        friend class chimera::ParameterTypeSystem;
        public:
            LuaFunctionWrapper(const LuaFunctionWrapper& wrapper);
            bool intern() const;
            virtual ~LuaFunctionWrapper();
            vec_t_LuaItem operator()(vec_t_LuaItem& params) const;
        protected:
        private:
            LuaFunctionWrapper(ChimeraSystem* sys);
            LuaFunctionWrapper(ChimeraSystem* sys, fn_luafnwrapper fn);
            void const * const getOrigin() const;
            ChimeraSystem* _chimeraSystem;
            fn_luafnwrapper _fn;
            void* _origin;

            friend int chimera::luat_luafunction_push(lua_State* const L);
            friend int chimera::luat_luafunction_delete(lua_State* const L);
    };
}

#endif // LUAFUNCTIONWRAPPER_H
