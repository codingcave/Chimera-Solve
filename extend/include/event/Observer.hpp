#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;
        class NotificationManager;

        class Observer
        {
            public:
                Observer();
                virtual ~Observer();
                virtual void trigger(NotificationManager* sender, void* args) = 0;
                virtual void addListener(chimera::simulation::IEventListener* listener) = 0;
                virtual void addListener(chimera::simulation::IEventListenerProvider* listener) = 0;
                virtual void removeListener(chimera::simulation::IEventListener* listener) = 0;
                virtual bool empty() const = 0;
            protected:
            private:
        };
    }
}

#endif // EVENTMANAGER_H
