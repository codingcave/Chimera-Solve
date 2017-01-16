#ifndef INTEGRATORREGISTRY_H
#define INTEGRATORREGISTRY_H


class IntegratorRegistry:
    public Registry
{
    public:
        IntegratorRegistry();
        virtual ~IntegratorRegistry();
        virtual bool checkFeatures(const map_t_size& features) const = 0;
        virtual const std::string getSystemName() const = 0;
        void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual AbstractIntegrator* getIntegratorInstance(vec_t_LuaItem& parameters) const = 0;
        inline const std::string getEntrypoint() const override { return Naming::EntryPoint_integrator; }
    protected:
    private:
};

#endif // INTEGRATORREGISTRY_H
