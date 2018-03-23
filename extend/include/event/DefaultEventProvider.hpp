#ifndef DEFAULTEVENTPROVIDER_H
#define DEFAULTEVENTPROVIDER_H

namespace chimera {
    namespace simulation {
        class DefaultEventProvider:
            public AbstractEventProvider
        {
            public:
                DefaultEventProvider(const std::string& name, Observer* observer);
                virtual ~DefaultEventProvider();
                virtual Observer* getObserver(NotificationManager*) override;
                virtual bool triggerCondition(NotificationManager*) override;
                virtual void* getEventArgs(NotificationManager*) override;
                virtual std::string getName() const override;
            protected:
            private:
                std::string _name;
                Observer* _observer;
        };
    }
}

#endif // DEFAULTEVENTPROVIDER_H
