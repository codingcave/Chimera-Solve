#ifndef LUAPARSER_H
#define LUAPARSER_H

class ModuleLoader;

class LuaParser: public IConnectEventHandler
{
    public:
        LuaParser(const std::string& filename, std::vector<char*>& argv, ModuleLoader* loader);
        virtual ~LuaParser();
        void notifyLoad(EntryPoint *ep) override;
        void notifyUnload(EntryPoint *ep) override;
        void notifyLoad(const std::string& name, EntryPoint *ep, Registry const * const reg) override;
        void notifyUnload(const std::string& name, EntryPoint *ep, Registry * const reg) override;
        void notifyLoad(ParameterType const * const p) override;
        void notifyUnload(ParameterType const * const p) override;

        void pushLuaValue(ParameterValue& param);
        void pushLuaValue(int type, void* value);
        void pushLuaValue(const std::string& type, void* value);

        void pushEntryPoint(EntryPoint* ep);
        void pushRegistry(const std::string& name, Registry const * const reg);
        bool processFile(void);
        lua_State* getLuaState() override;
    protected:
    private:
        std::string* _filename;
        lua_State* _L;
        lua_State* _rL;
        //SimulationFeatures* _features;
        ModuleLoader* _loader;
        //map_t_Dict* _params;
        //map_t_Types* _paramTypes;
        void init();
        void chainLoad(ParameterType const * const p, const int& id, const std::string& metaName) const;
        friend int lua_Simulation_start(lua_State* L);
};

#endif // LUAPARSER_H
