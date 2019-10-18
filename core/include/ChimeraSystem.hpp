#ifndef CHIMERASYSTEM_H
#define CHIMERASYSTEM_H

namespace chimera {
    class ChimeraSystem:
        public StateSynchrony
    {
        friend class chimera::EntryPoint;
        friend class chimera::EntryPointSystem;
        friend class chimera::ParameterTypeSystem;

        public:
            ChimeraSystem();
            virtual ~ChimeraSystem();

            LoggingSystem* getLoggingSystem();
            ParameterTypeSystem* getTypeSystem();
            EntryPointSystem* getEntryPointSystem();
            Module* includeModule(EntryPoint* entrypoint, const std::string& name);
            virtual Module* openModule(EntryPoint const * const entrypoint, const std::string& name);
            virtual void closeModule(Module* module);

            lua_State* getLuaState();

            virtual const std::string getGUID() const override;
        protected:
            void pushEntryPoint(const std::string& name, EntryPoint* ep);
            void popEntryPoint(const std::string& name, EntryPoint* ep);
            void pushModule(EntryPoint* ep, const std::string& name, Module const * const module);
            void popModule(EntryPoint* ep, Module const * const module);
            void pushType(ParameterType const * const p);
            void popType(const ParameterType& type);
            void pushLuaValue(lua_State* const L, chimera::ParameterValue& param) const;
            void pushLuaValue(lua_State* const L, size_t type, void* value) const;
            void pushLuaValue(lua_State* const L, const std::string& type, void* value) const;
            void deleteLuaValue(lua_State* const L, ParameterValue& value) const;
            void deleteLuaValue(lua_State* const L, size_t type, void* value) const;
            void pushLibrary(const std::string& name, lua_CFunction createMetaTable);
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag);
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef);
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags);
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& flags);
            void setLogLevel(const unsigned int& loglevel);
        private:
            lua_State* _L;

            void init();
            virtual void notifyItemAdded(StateSynchrony* sender, void const * const item, void const * const data) override;
            virtual void notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data) override;
            virtual void notifyItemLoaded(StateSynchrony* sender, void const * const item, void const * const data) override;
            virtual void notifyItemUnloaded(StateSynchrony* sender, void const * const item, void const * const data) override;

            void chainLoad(ParameterType const * const p, const int& id) const;

            LoggingSystem* _loggingSys;
            ParameterTypeSystem* _typeSys;
            EntryPointSystem* _epSys;
    };
}

#endif // CHIMERASYSTEM_H
