#ifndef RUNGEKUTTA_DOUBLE_VECVECDOUBLE_H
#define RUNGEKUTTA_DOUBLE_VECVECDOUBLE_H

typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > vec_vec_real;
namespace odeint = boost::numeric::odeint;

//vec_vec_real& operator+=(vec_vec_real &v1, const vec_vec_real &v2);
//vec_vec_real& operator*=(vec_vec_real &v, const double a);

vec_vec_real operator*(const double a, const vec_vec_real &v);
vec_vec_real operator+(const vec_vec_real &a, const vec_vec_real &v);

class RungeKutta_double_vecvecDouble:
    public chimera::simulation::TemplateIntegrator<double, vec_vec_real>
{
    public:
        RungeKutta_double_vecvecDouble(chimera::ParameterTypeSystem* ps, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, vec_vec_real>* system, double dt);
        //RungeKutta_double_vecvecDouble();
        virtual ~RungeKutta_double_vecvecDouble();
        virtual bool nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual const double& getTime() const override;
        virtual const vec_vec_real& getState() const override;

        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(chimera::vec_t_LuaItem args) override;
        virtual void start(chimera::vec_t_LuaItem args) override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        odeint::runge_kutta4<vec_vec_real>* _rk;
        RungeKuttaSystem<double, vec_vec_real>* _system;
        double _time;
        double _endtime;
        double _dt;
        vec_vec_real* _state;
        const size_t _init;
};

#endif // RUNGEKUTTA_DOUBLE_VECVECDOUBLE_H
