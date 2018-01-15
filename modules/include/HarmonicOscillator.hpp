#ifndef HARMONICOSCILLATOR_H
#define HARMONICOSCILLATOR_H

class HarmonicOscillatorModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        HarmonicOscillatorModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getVersion() const override;
};

class HarmonicOscillator:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >
{
    public:
        explicit HarmonicOscillator(chimera::ParameterTypeSystem* ps, double a, double b);
        virtual ~HarmonicOscillator();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

        double _a;
        double _b;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
};

#endif // HARMONICOSCILLATOR_H
