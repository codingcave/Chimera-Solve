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

template<typename state_type, typename time_type>
class RungeKuttaSystem {
    public:
        RungeKuttaSystem(TemplateOdeSystem<state_type, time_type>* system)
        {
            _system = system;
        }
        virtual ~RungeKuttaSystem() {}
        void operator()(const state_type& x, state_type& dxdt, const time_type& t)
        {
            return (*_system)(x, dxdt, t);
        }
    private:
        TemplateOdeSystem<state_type, time_type>* _system;
};

class RungeKutta_double_vecDouble:
    //public TemplateIntegrator<struct T_VectorDef, double>
    public AbstractIntegrator
{
    public:
        RungeKutta_double_vecDouble(TemplateOdeSystem<boost::numeric::ublas::vector<double>, double>* system, double dt);
        //RungeKutta_double_vecDouble();
        virtual ~RungeKutta_double_vecDouble();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        //virtual double * getTime() override;
        //virtual vec_vec_real* getState() override;
        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(vec_t_LuaItem args) override;
        virtual void start(vec_t_LuaItem args) override;
    protected:
    private:
        odeint::runge_kutta4<boost::numeric::ublas::vector<double> >* _rk;
        RungeKuttaSystem<boost::numeric::ublas::vector<double>, double>* _system;
        double _time;
        double _endtime;
        double _dt;
        boost::numeric::ublas::vector<double>* _state;
};

class RungeKutta_double_vecvecDouble:
    //public TemplateIntegrator<struct T_VectorDef, double>
    public AbstractIntegrator
{
    public:
        RungeKutta_double_vecvecDouble(TemplateOdeSystem<vec_vec_real, double>* system);
        //RungeKutta_double_vecvecDouble();
        virtual ~RungeKutta_double_vecvecDouble();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        //virtual double * getTime() override;
        //virtual vec_vec_real* getState() override;
        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(vec_t_LuaItem args) override;
        virtual void start(vec_t_LuaItem args) override;
    protected:
    private:
        odeint::runge_kutta4<vec_vec_real>* _rk;
        TemplateOdeSystem<vec_vec_real, double>* _system;
        double _time;
        vec_vec_real* _state;
        struct T_VectorDef* _vd;
};

#endif // RUNGEKUTTA_H
