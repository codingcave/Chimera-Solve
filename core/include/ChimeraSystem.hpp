#ifndef CHIMERASYSTEM_H
#define CHIMERASYSTEM_H

/*
class ParameterType;
class EntryPoint;
class Module;
class LoggingSystem;
class ParameterTypeSystem;
class EntryPointSystem;
class ParameterValue;
*/
namespace chimera {
    int lua_global_scope_newindex(lua_State* L);

    class ChimeraSystem:
        public StateSynchrony
    //:public IConnectEventHandler
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
            //ChimeraSystem(ParameterTypeSystem* typeSys, EntryPointSystem* epSys, LoggingSystem* logSys);
            void pushEntryPoint(const std::string& name, EntryPoint* ep);
            void popEntryPoint(const std::string& name, EntryPoint* ep);
            void pushModule(EntryPoint* ep, const std::string& name, Module const * const reg);
            void popModule(EntryPoint* ep, Module const * const reg);
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
            /*
            void notifyLoad(EntryPoint *ep);
            void notifyUnload(EntryPoint *ep);
            void notifyLoad(const std::string& name, EntryPoint *ep, Module const * const reg);
            void notifyUnload(const std::string& name, EntryPoint *ep, Module * const reg);
            void notifyLoad(ParameterType const * const p);
            void notifyUnload(ParameterType const * const p);
            */
            //virtual void notifyLoad(StateSynchrony* sender, void const * const data);
            //virtual void notifyUnload(StateSynchrony* sender, void const * const data);
            //virtual void notifyChange(StateSynchrony* sender, void const * const data);
            virtual void notifyItemAdded(StateSynchrony* sender, void const * const item, void const * const data) override;
            virtual void notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data) override;
            virtual void notifyItemLoaded(StateSynchrony* sender, void const * const item, void const * const data) override;
            virtual void notifyItemUnloaded(StateSynchrony* sender, void const * const item, void const * const data) override;
            //virtual void notifyDelete(StateSynchrony* sender);

            void chainLoad(ParameterType const * const p, const int& id) const;

            LoggingSystem* _loggingSys;
            ParameterTypeSystem* _typeSys;
            EntryPointSystem* _epSys;
    };
}

#endif // CHIMERASYSTEM_H
