#ifndef EULERMODULE_H
#define EULERMODULE_H

class EulerModule:
    public chimera::simulation::IntegratorModule
    //public Registry
{
    public:
        EulerModule();
        virtual ~EulerModule();
        virtual bool checkFeatures(const chimera::map_t_size& features) const override;
        virtual const std::string getSystemName() const override;
        virtual chimera::simulation::AbstractIntegrator* getIntegratorInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual const std::string getVersion() const override;
        //virtual void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getGUID() const override;
        virtual void load(chimera::EntryPoint const * const entryPoint, void const * const params) override;
    protected:
    private:
        chimera::EntryPoint* _init;
        static constexpr double DEFAULT_DT = 0.01;
        chimera::simulation::AbstractIntegrator* getSystem(const chimera::ParameterValue& param, const double& dt) const;
};

#endif // EULERMODULE_H
