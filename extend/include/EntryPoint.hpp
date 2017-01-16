#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

class EntryPoint;
class Registry;

typedef std::unordered_map<std::string, EntryPoint*> map_t_EntryPoint;
typedef std::unordered_map<std::string, Registry*> map_t_Registry;

class EntryPoint:public ItemContainer
{
    friend class LuaParser;
    friend class EntryPointSystem;
    public:
        EntryPoint();
        virtual ~EntryPoint();
        class iterator;
        //const struct T_Parameter* getValue(const std::string& name) const;
        virtual const std::string getName() const = 0;
        virtual const std::string getVersion() const = 0;
        bool load(const std::string& name, Registry * const reg);
        const Registry* getRegistry(const std::string& name) const;
        int size() const;
        friend std::ostream& operator<< (std::ostream &out, const EntryPoint &d);
    protected:
        virtual bool onLoad(Registry const * const reg);
        virtual void init(lua_State* L);
    private:
        map_t_Registry *_loadedRegistries;
        void unload(const std::string& name);
        void unloadAll();
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

#endif // ENTRYPOINT_H
