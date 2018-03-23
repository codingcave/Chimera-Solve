#ifndef KURAMOTO_H
#define KURAMOTO_H

class KuramotoModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        KuramotoModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getVersion() const override;
};

class KuramotoOscillator:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >
{
    public:
        explicit KuramotoOscillator(chimera::ParameterTypeSystem* ps, int N, double omega);
        virtual ~KuramotoOscillator();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

        int _N;
        double _omega;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
};

#endif // KURAMOTO_H
