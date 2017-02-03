#ifndef FITZHUGHNAGUMO_H
#define FITZHUGHNAGUMO_H

class FitzHughNagumo:
    //template<typename state_type, typename time_type>
    public TemplateOdeSystem<boost::numeric::ublas::vector<double>, double>
{
    public:
        explicit FitzHughNagumo(double epsilon, double a);
        virtual ~FitzHughNagumo();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;
        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

        double _epsilon;
        double _a;
    protected:
    private:

};


class FitzHughNagumoRegistry:
    public Registry
{
    public:
        FitzHughNagumoRegistry();
        virtual const std::string getEntrypoint() const override;
        virtual void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getVersion() const override;
};

#endif // FITZHUGHNAGUMO_H
