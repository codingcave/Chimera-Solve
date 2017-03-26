#ifndef RUNGEKUTTA_H
#define RUNGEKUTTA_H

namespace odeint = boost::numeric::odeint;

typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > vec_vec_real;

class RungeKuttaRegistry:
    public IntegratorRegistry
    //public Registry
{
    public:
        RungeKuttaRegistry();
        virtual ~RungeKuttaRegistry();
        virtual bool checkFeatures(const map_t_size& features) const override;
        virtual const std::string getSystemName() const override;
        virtual AbstractIntegrator* getIntegratorInstance(vec_t_LuaItem& parameters) const override;
        virtual const std::string getVersion() const override;
        //virtual void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;

    protected:
    private:
};

template<typename time_type, typename state_type>
class RungeKuttaSystem {
    public:
        RungeKuttaSystem(TemplateOdeSystem<time_type, state_type>* system)
        {
            _system = system;
        }
        virtual ~RungeKuttaSystem() {}
        void operator()(const state_type& x, state_type& dxdt, const time_type& t)
        {
            return (*_system)(x, dxdt, t);
        }
    private:
        TemplateOdeSystem<time_type, state_type>* _system;
};


class RungeKutta_double_vecDouble:
    public TemplateIntegrator
    <double, boost::numeric::ublas::vector<double> >
    //public AbstractIntegrator
{
    public:
        RungeKutta_double_vecDouble(TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* system, double dt);
        //RungeKutta_double_vecDouble();
        virtual ~RungeKutta_double_vecDouble();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual const double& getTime() const override;
        virtual const boost::numeric::ublas::vector<double>& getState() const override;

        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(vec_t_LuaItem args) override;
        virtual void start(vec_t_LuaItem args) override;
    protected:
    private:
        odeint::runge_kutta4<boost::numeric::ublas::vector<double> >* _rk;
        RungeKuttaSystem<double, boost::numeric::ublas::vector<double> >* _system;
        double _time;
        double _endtime;
        double _dt;
        boost::numeric::ublas::vector<double>* _state;
};

/*
class RungeKutta_double_vecvecDouble:
    public TemplateIntegrator<double, vec_vec_real>
    //public AbstractIntegrator
{
    public:
        RungeKutta_double_vecvecDouble(TemplateOdeSystem<double, vec_vec_real>* system, double dt);
        //RungeKutta_double_vecvecDouble();
        virtual ~RungeKutta_double_vecvecDouble();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        //virtual double * getTime() override;
        //virtual vec_vec_real* getState() override;
        virtual const double& getTime() const override;
        virtual const vec_vec_real& getState() const override;

        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(vec_t_LuaItem args) override;
        virtual void start(vec_t_LuaItem args) override;
    protected:
    private:
        odeint::runge_kutta4<vec_vec_real>* _rk;
        RungeKuttaSystem<double, vec_vec_real>* _system;
        double _time;
        double _endtime;
        double _dt;
        vec_vec_real* _state;
};
*/
#endif // RUNGEKUTTA_H
