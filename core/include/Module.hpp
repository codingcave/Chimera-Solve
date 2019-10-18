#ifndef REGISTRY_H
#define REGISTRY_H

namespace chimera {
    class EntryPoint;

    class Module:
        public StateSynchrony,
        public ItemContainer
    {
        public:
            friend class EntryPoint;
            friend class EntryPointSystem;
            friend class ChimeraSystem;
            class iterator;
            Module();
            virtual ~Module();

            //virtual const std::string getGUID() const = 0;
            virtual const std::string getVersion() const = 0;
            virtual void* getInstance(vec_t_LuaItem& parameters) const = 0;
            virtual void destroyInstance(void * const instance) const = 0;
            virtual void load(EntryPoint const * const entryPoint, void const * const params);
            virtual void unload();
            virtual size_t getFlag(const std::string& flag) const;
            ChimeraSystem* getChimeraSystem() const override;
            // size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef);
            // size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag);
            // size_t NEW_PARAMETER<typename>(const std::string& name);
            // Add parameter with no init, default push, und delete (typename*)value;
            // --ParameterValue createValue(size_t type, void* value)
            // void addEntryPoint(const std::string& name, EntryPoint* ep);
        protected:
            void registerMethod(const std::string& name, chimera::fn_instancefnwrapper method);
        private:
            std::unordered_map<std::string, fn_instancefnwrapper> const * const methods() const;
            ChimeraSystem* _chimeraSystem;
            std::unordered_map<std::string, chimera::fn_instancefnwrapper>* _instanceMethods;

            void loadModule(EntryPoint const * const entryPoint, const std::string& name, void const * const params);
            void unloadModule();
            friend int chimera::lua_EntryPoint_call(lua_State*);
            friend int chimera::lua_Instance_index(lua_State*);
    };

    class Module::iterator
    {
        public:
            iterator (map_t_Module::iterator it);
            virtual ~iterator();
            Module* operator*();
            Module::iterator operator++(); // prefix
            Module::iterator operator++(int); // postfix
            bool operator==(const Module::iterator& other);
            bool operator!=(const Module::iterator& other);
        private:
            map_t_Module::iterator _it;
    };
}

#endif // REGISTRY_H
