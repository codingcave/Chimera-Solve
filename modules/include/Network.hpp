#ifndef NETWORK_H
#define NETWORK_H


class Network:
    //template<typename time_type, typename state_type>
    public TemplateOdeSystem<double, boost::numeric::ublas::vector<double>>
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


class NetworkRegistry:
    public Registry
{
    public:
        FitzHughNagumoRegistry();
        virtual const std::string getEntrypoint() const override;
        virtual void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
        virtual const std::string getVersion() const override;
};

#endif // NETWORK_H
