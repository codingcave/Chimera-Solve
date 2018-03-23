#ifndef ABSTRACTEVENTPROVIDER_H
#define ABSTRACTEVENTPROVIDER_H

namespace chimera {
    namespace simulation {
        class AbstractEventProvider
        {
            public:
                AbstractEventProvider();
                virtual ~AbstractEventProvider();
                virtual Observer* getObserver(NotificationManager* sender) = 0;
                virtual bool triggerCondition(NotificationManager* sender) = 0;
                virtual void* getEventArgs(NotificationManager* sender) = 0;
                virtual std::string getName() const = 0;
            protected:
            private:
        };

    }
}

#endif // ABSTRACTEVENTPROVIDER_H
