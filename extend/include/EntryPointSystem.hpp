#ifndef ENTRYPOINTSYSTEM_H
#define ENTRYPOINTSYSTEM_H

class IEntryPointEventHandler;

class EntryPointSystem final
{
    friend class EntryPoint;
    friend class LuaParser;
    public:
        virtual ~EntryPointSystem();
        static void addEntryPoint(EntryPoint* const ep);
        static EntryPoint* const get(const std::string& name);
        static EntryPoint::iterator begin();
        static EntryPoint::iterator end();
    protected:
    private:
        static EntryPointSystem _instance;
        static void connect(IConnectEventHandler* const c);
        static void disconnect();

        EntryPointSystem();
        map_t_EntryPoint* _hooks;
        IConnectEventHandler* _luaparser;
};

#endif // ENTRYPOINTSYSTEM_H
