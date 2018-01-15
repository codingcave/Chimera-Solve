#ifndef RUNGEKUTTA_H
#define RUNGEKUTTA_H

namespace odeint = boost::numeric::odeint;

class RungeKutta_double_vecDouble:
    public chimera::simulation::TemplateIntegrator
    <double, boost::numeric::ublas::vector<double> >
    //public AbstractIntegrator
{
    public:
        RungeKutta_double_vecDouble(chimera::ParameterTypeSystem* ps, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* system, double dt);
        //RungeKutta_double_vecDouble();
        virtual ~RungeKutta_double_vecDouble();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual const double& getTime() const override;
        virtual const boost::numeric::ublas::vector<double>& getState() const override;

        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(chimera::vec_t_LuaItem args) override;
        virtual void start(chimera::vec_t_LuaItem args) override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        odeint::runge_kutta4<boost::numeric::ublas::vector<double> >* _rk;
        RungeKuttaSystem<double, boost::numeric::ublas::vector<double> >* _system;
        double _time;
        double _endtime;
        double _dt;
        boost::numeric::ublas::vector<double>* _state;
        size_t _init;
};

#endif // RUNGEKUTTA_H
