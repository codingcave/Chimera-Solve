#ifndef NETWORK_DOUBLE_VECCOMPLEX_H
#define NETWORK_DOUBLE_VECCOMPLEX_H

class Network_double_vecComplex:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > > >,
    public INetwork<double, boost::numeric::ublas::vector<std::complex<double> > >
{
    public:
        Network_double_vecComplex(chimera::ParameterTypeSystem* ps, chimera::ParameterValue& unit, const int& number, chimera::ParameterValue& coupling);

        virtual ~Network_double_vecComplex();

        virtual chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* getNode(int i) const override;

        virtual void operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > >& x, boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > >& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* _unitSys;
        chimera::ParameterValue* _unitValue;
        int _number;
        TemplateCoupling<double, boost::numeric::ublas::vector<std::complex<double> > >* _coupling;
        chimera::ParameterValue* _couplingValue;
        boost::numeric::ublas::vector<std::complex<double> >* _tmp;
};

#endif // NETWORK_DOUBLE_VECCOMPLEX_H