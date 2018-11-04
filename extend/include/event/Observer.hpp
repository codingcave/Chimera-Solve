#ifndef OBSERVER_H
#define OBSERVER_H

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
                void trigger(NotificationManager* sender, void* args);
                void addListener(chimera::simulation::IEventListener* listener);
               void removeListener(chimera::simulation::IEventListener* listener);
                bool empty() const;
            protected:
            private:
                std::unordered_set<IEventListener*>* _listeners;
        };
    }
}

#endif // OBSERVER_H
