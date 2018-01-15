#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;

        class EventManager
        {
            public:
                EventManager();
                EventManager(void* owner);
                virtual ~EventManager();
                virtual void trigger(void* sender, void* args) = 0;
                virtual void addListener(chimera::simulation::IEventListener* listener) = 0;
                virtual void addListener(chimera::simulation::IEventListenerProvider* listener) = 0;
                virtual void removeListener(chimera::simulation::IEventListener* listener) = 0;
                virtual void const * const getOwner() const;
            protected:
                void* _owner;
            private:
        };
    }
}

#endif // EVENTMANAGER_H
