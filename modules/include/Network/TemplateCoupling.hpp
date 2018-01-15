#ifndef TEMPLATECOUPLING_H
#define TEMPLATECOUPLING_H

template<typename inner_state_type>
class TemplateCoupling:
    public AbstractCoupling
{
    public:
        TemplateCoupling() {}
        virtual ~TemplateCoupling() {}
        virtual void operator()(const boost::numeric::ublas::vector<inner_state_type>& system, int i, int j, inner_state_type& out) = 0;
    protected:
    private:
};

#endif // TEMPLATECOUPLING_H
