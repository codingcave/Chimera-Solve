#ifndef TEMPLATEINTEGRATOR_H
#define TEMPLATEINTEGRATOR_H

template<typename state_type, typename time_type>
class TemplateIntegrator:
    public AbstractIntegrator
{
    public:
        TemplateIntegrator(){}
        virtual ~TemplateIntegrator(){}

        void * currentTime() override { return (void *)getTime(); }
        void * currentState() override { return (void *)getState(); }
        virtual time_type * getTime() = 0;
        virtual state_type * getState() = 0;

    protected:
    private:
};

#endif // TEMPLATEINTEGRATOR_H
