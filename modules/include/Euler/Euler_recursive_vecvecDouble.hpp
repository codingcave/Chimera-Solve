#ifndef EULER_RECURSIVE_VECVECDOUBLE_H
#define EULER_RECURSIVE_VECVECDOUBLE_H

typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > vec_vec_real;

class Euler_recursive_vecvecDouble:
    public chimera::simulation::TemplateIntegrator
    <double, vec_vec_real >
    //public AbstractIntegrator
{
    public:
        Euler_recursive_vecvecDouble(chimera::ParameterTypeSystem* ps, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, vec_vec_real >* system);
        virtual ~Euler_recursive_vecvecDouble();
        virtual void nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual const double& getTime() const override;
        virtual const vec_vec_real& getState() const override;

        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(chimera::vec_t_LuaItem args) override;
        virtual void start(chimera::vec_t_LuaItem args) override;
        virtual bool finished() const override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        chimera::simulation::TemplateOdeSystem<double, vec_vec_real >* _system;
        double _time;
        double _endtime;
        vec_vec_real* _state;
        vec_vec_real* _swapState;
        size_t _init;
};


#endif // EULER_RECURSIVE_VECVECDOUBLE_H
