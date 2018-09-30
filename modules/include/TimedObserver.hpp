#ifndef TIMEDOBSERVER_H
#define TIMEDOBSERVER_H

class TimedObserverModule:
    public chimera::simulation::ObserverModule
{
    public:
        TimedObserverModule();
        virtual ~TimedObserverModule();
        virtual const std::string getGUID() const override;
        virtual const std::string getVersion() const override;
        virtual chimera::simulation::AbstractEventProvider* getEventInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
};

class TimedObserverProvider:
    public chimera::simulation::AbstractEventProvider
{
    public:
        explicit TimedObserverProvider(double start, double step, double end);
        virtual ~TimedObserverProvider();
        virtual chimera::simulation::Observer* getObserver(chimera::simulation::NotificationManager* sender) override;
        virtual bool triggerCondition(chimera::simulation::NotificationManager* sender) override;
        virtual void* getEventArgs(chimera::simulation::NotificationManager* sender) override;
        virtual std::string getName() const override;
    protected:
    private:
        double _start;
        double _step;
        double _end;
        double _last;
        struct chimera::simulation::T_TimeStateArgs<double, boost::numeric::ublas::vector<double> >* _args1;
        struct chimera::simulation::T_TimeStateArgs<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >* _args2;
        struct chimera::simulation::T_TimeStateArgs<double, boost::numeric::ublas::vector<std::complex<double> > >* _args3;
        struct chimera::simulation::T_TimeStateArgs<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > > >* _args4;
};

#endif // TIMEDOBSERVER_H
