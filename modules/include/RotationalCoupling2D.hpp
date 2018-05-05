#ifndef ROTATIONALCOUPLING2D_H
#define ROTATIONALCOUPLING2D_H

class RotationalCoupling2DModule:
    public CouplingModule
    //public Registry
{
    public:
        RotationalCoupling2DModule();
        virtual ~RotationalCoupling2DModule();
        virtual const std::string getVersion() const override;
        virtual AbstractCoupling* getCoupling(chimera::vec_t_LuaItem& parameters) const override;
        virtual const std::string getGUID() const override;
    protected:
    private:
};

class RotationalCoupling2D
    :public TemplateCoupling
    <double, boost::numeric::ublas::vector<double> >
{
    public:
        RotationalCoupling2D(double epsilon, double sigma, double phi, chimera::ParameterValue couplingValue);
        virtual ~RotationalCoupling2D();
        virtual void operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const double& t, const int& i, boost::numeric::ublas::vector<double>& out) override;
    protected:
    private:
        double _epsilon;
        double _sigma;
        double _phi;
        double _sinPhi;
        double _cosPhi;
        boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >* _couplingMatrix;
        chimera::ParameterValue* _couplingValue;
};

#endif // ROTATIONALCOUPLING2D_H