#ifndef REGISTRY_H
#define REGISTRY_H

class Registry:public ItemContainer
{
    public:
        Registry();
        virtual ~Registry();

        virtual const std::string getEntrypoint() const = 0;
        virtual const std::string getVersion() const = 0;
        virtual void* getInstance(vec_t_LuaItem& parameters) const = 0;
        virtual void destroyInstance(void * const instance) const = 0;
        void registerMethod(const std::string& name, lua_CFunction method);
        std::unordered_map<std::string, lua_CFunction> const * const methods() const;
    protected:
    private:
        std::unordered_map<std::string, lua_CFunction>* _instanceMethods;
};

#endif // REGISTRY_H
