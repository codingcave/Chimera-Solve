#ifndef RUNGEKUTTA_H
#define RUNGEKUTTA_H

namespace odeint = boost::numeric::odeint;

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
        virtual const std::string getEntrypoint() const override;
        virtual const std::string getVersion() const override;
        //virtual void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;

    protected:
    private:
};

class RungeKutta:
    public TemplateIntegrator<double, double>
    //public AbstractIntegrator
{
    public:
        RungeKutta(TemplateOdeSystem<double, double>* system);
        //RungeKutta();
        virtual ~RungeKutta();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual double * getTime() override;
        virtual double * getState() override;
        //virtual void * currentTime() override;
        //virtual void * currentState() override;
        virtual void initialize(vec_t_LuaItem args) override;

    protected:
    private:
        odeint::runge_kutta4< double >* _rk;
        TemplateOdeSystem<double, double>* _system;
        double _time;
        double _state;
};

#endif // RUNGEKUTTA_H
