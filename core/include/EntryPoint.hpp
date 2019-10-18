#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

namespace chimera {
    class Module;

    class EntryPoint:public StateSynchrony, public ItemContainer
    {
        friend class ChimeraSystem;
        friend class EntryPointSystem;
        public:
            EntryPoint();
            virtual ~EntryPoint();
            class iterator;
            //const struct T_Parameter* getValue(const std::string& name) const;
            //virtual const std::string getGUID() const = 0;
            virtual const std::string getVersion() const = 0;
            bool addModule(const std::string& name, Module * const module);
            void removeModule(const std::string& name);
            const std::string findModule(Module const * const module) const;
            const Module* getModule(const std::string& name) const;
            int size() const;
            Module::iterator begin();
            Module::iterator end();
            ChimeraSystem* getChimeraSystem() const override;
            virtual void load();
            virtual void unload();
            friend std::ostream& operator<< (std::ostream &out, const EntryPoint &d);
            friend int chimera::lua_EntryPoint_call(lua_State* L);
        protected:
            virtual bool checkModule(Module const * const module) const;
            virtual void init(lua_State* L);
            virtual void const * const getModuleLoadParams() const;
        private:
            ChimeraSystem* _chimeraSystem;
            map_t_Module *_loadedModules;
            void loadEntryPoint(ChimeraSystem * chimerasystem);
            void unloadEntryPoint();
            //virtual void notifyUnload(StateSynchrony* sender, void const * const data);
            virtual void notifyDelete(StateSynchrony* sender);
    };

    class EntryPoint::iterator
    {
        public:
            iterator (map_t_EntryPoint::iterator it);
            virtual ~iterator();
            EntryPoint* operator*();
            EntryPoint::iterator operator++(); // prefix
            EntryPoint::iterator operator++(int); // postfix
            bool operator==(const EntryPoint::iterator& other);
            bool operator!=(const EntryPoint::iterator& other);
        private:
            map_t_EntryPoint::iterator _it;
    };
}

#endif // ENTRYPOINT_H
