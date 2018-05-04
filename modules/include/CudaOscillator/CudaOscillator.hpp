#ifndef CUDAOSCILLATOR_H
#define CUDAOSCILLATOR_H

class CudaOscillatorModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        CudaOscillatorModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getVersion() const override;
};

class CudaOscillator:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >
{
    public:
        explicit CudaOscillator(chimera::ParameterTypeSystem* ps, double a, double b);
        virtual ~CudaOscillator();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

        double _a;
        double _b;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
};

#endif // CUDAOSCILLATOR_H
