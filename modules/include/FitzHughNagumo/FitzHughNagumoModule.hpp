#ifndef FITZHUGHNAGUMO_REGISTRY_H
#define FITZHUGHNAGUMO_REGISTRY_H

class FitzHughNagumoModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        FitzHughNagumoModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getVersion() const override;
};

#endif // FITZHUGHNAGUMO_REGISTRY_H
