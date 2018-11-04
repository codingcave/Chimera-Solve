#ifndef DEFAULTEVENTPROVIDER_H
#define DEFAULTEVENTPROVIDER_H

namespace chimera {
    namespace simulation {
        class DefaultEventProvider:
            public AbstractEventProvider
        {
            public:
                DefaultEventProvider();
                virtual ~DefaultEventProvider();
            protected:
                virtual bool triggerCondition(NotificationManager const * const) override;
                virtual void* getEventArgs(NotificationManager const * const) override;
            private:
        };
    }
}

#endif // DEFAULTEVENTPROVIDER_H
