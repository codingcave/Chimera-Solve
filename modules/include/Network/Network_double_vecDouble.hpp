#ifndef NETWORK_H
#define NETWORK_H

class Network_double_vecDouble:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >
{
    public:
        Network_double_vecDouble(chimera::ParameterTypeSystem* ps, chimera::ParameterValue& unit, const int& number, chimera::ParameterValue& coupling);

        virtual ~Network_double_vecDouble();

        virtual void operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& x, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* _unitSys;
        chimera::ParameterValue* _unitValue;
        int _number;
        TemplateCoupling<boost::numeric::ublas::vector<double> >* _coupling;
        chimera::ParameterValue* _couplingValue;
};

#endif // NETWORK_H
