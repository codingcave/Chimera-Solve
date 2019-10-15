#ifndef PARAMETERTYPESYSTEM_H
#define PARAMETERTYPESYSTEM_H

typedef int (*lua_CFunction) (lua_State *L);

namespace chimera {
    //class IConnectEventHandler;

    class ParameterValue;

    class ParameterTypeSystem:
        public StateSynchrony
    {
        friend class ChimeraSystem;
        friend class LuaFunctionWrapper;
        friend class ParameterValue;
        friend class ParameterReferenceString;
        friend class ParameterReferenceInteger;
        public:
            virtual ~ParameterTypeSystem();
            size_t getParameterID(const std::string& name) const;
            //size_t getParameterID(const size_t& base, const size_t& tag);
            //size_t[] filterIDs(const size_t& base, const size_t& tag, int& cnt);
            const std::string getParameterName(const size_t& id) const;
            const bool exists(const size_t& id) const;
            size_t getParameterBase(const size_t& id) const;
            size_t getParameterTag(const size_t& id) const;
            ParameterValue createValue(size_t type, void* value) const;
            LuaFunctionWrapper createFunction(fn_luafnwrapper fn) const;
            //static bool pushValue(ParameterValue& value);

            //static void deleteValue(const struct T_Parameter& value);
            inline bool isParameterID(const std::string& name, const size_t& id) const
            {
                return id == getParameterID(name);
            }
            bool isReferenced(void* ptr) const;

            friend std::ostream& operator<<(std::ostream& os, const ParameterValue& obj);

            virtual const std::string getGUID() const override;
            ParameterValue getValue(lua_State* const L, const int& index);
            size_t getParameterType(lua_State* const L, const int& index) const;

            void addDependency(void* item, void* dependency);
            void releaseDependency(void* item, void* dependency);
            void removeDependencyItem(void* item);
        protected:
            bool pushValue(lua_State* const L, struct T_Parameter value);
            bool pushValue(lua_State* const L, size_t type, void* value);
            //bool pushReal(lua_State* const L, const double& value) const;
            //bool pushString(lua_State* const L, const std::string& value) const;
            //bool pushBoolean(lua_State* const L, const bool& value) const;
            //bool pushFunction(lua_State* const L, lua_CFunction value) const;
            void deleteValue(lua_State* const L, ParameterValue& value) const;
            void deleteValue(lua_State* const L, size_t type, void* value) const;
            ParameterType const * const getParameter(const size_t& id) const;
            void removeParameter(const std::string& name);
            void removeParameter(const size_t& id);
            void removeParameter(void const * const origin);
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag);
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef);
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags);
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& flags);
        private:
            friend int chimera::lua_UserData_gc(lua_State* L);
            friend int chimera::luat_luafunction_call(lua_State* L);
            friend int chimera::luat_table_push(lua_State* const L);
            friend int chimera::lua_Module_call(lua_State* L);
            friend int chimera::lua_Instance_forwardMethod(lua_State* L);
            ChimeraSystem* _chimeraSystem;
            size_t _lastID;
            std::vector<ParameterType*>* _typeList;
            std::unordered_map<void*, ParameterValue>* _references;

            std::unordered_map<void*, std::unordered_set<void*> >* _dependencies;
            std::unordered_map<void*, std::unordered_set<void*> >* _reverseDependencies;

            ParameterTypeSystem();
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags, bool notify);
            void deleteAllReferences(size_t type);
            void disposeReference(void* value);
            //void notifyUnload(StateSynchrony* sender, void const * const data) override;
            //void notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data) override;
            //static void finalizeValue(lua_State* const L, ParameterValue& value);
            void addBaseTypes();

            friend int chimera::luat_table_push(lua_State* const L);
    };
}

#endif // PARAMETERTYPESYSTEM_H
