#ifndef TEMPLATESYSTEMDYNAMIC_H
#define TEMPLATESYSTEMDYNAMIC_H

template<typename state_type, typename time_type>
class TemplateOdeSystem:
    public AbstractSystemDynamic
{
    public:
        TemplateOdeSystem();
        virtual ~TemplateOdeSystem();
        virtual void operator()(const state_type& x, state_type& dxdt, const time_type& t) = 0;
    protected:
    private:
};

#endif // TEMPLATESYSTEMDYNAMIC_H
