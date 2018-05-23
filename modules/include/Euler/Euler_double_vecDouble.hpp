#ifndef EULER_DOUBLE_VECDOUBLE_H
#define EULER_DOUBLE_VECDOUBLE_H

namespace odeint = boost::numeric::odeint;

class Euler_double_vecDouble:
    public chimera::simulation::TemplateIntegrator
    <double, boost::numeric::ublas::vector<double> >
    //public AbstractIntegrator
{
    public:
        Euler_double_vecDouble(chimera::ParameterTypeSystem* ps, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* system, double dt);
        virtual ~Euler_double_vecDouble();
        virtual void nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual const double& getTime() const override;
        virtual const boost::numeric::ublas::vector<double>& getState() const override;

        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(chimera::vec_t_LuaItem args) override;
        virtual void start(chimera::vec_t_LuaItem args) override;
        virtual bool finished() const override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        odeint::euler<boost::numeric::ublas::vector<double> >* _euler;
        EulerSystem<double, boost::numeric::ublas::vector<double> >* _system;
        double _time;
        double _endtime;
        double _dt;
        boost::numeric::ublas::vector<double>* _state;
        size_t _init;
};

#endif // EULER_DOUBLE_VECDOUBLE_H
