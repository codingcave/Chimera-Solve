#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

namespace chimera {
    namespace simulation {
        class AbstractEventProvider;
        class IEventListener;
        class IEventListenerProvider;

        class NotificationManager
        {
            public:
                NotificationManager();
                virtual ~NotificationManager();
                void registerEvent(const std::string& name, chimera::simulation::Observer* evList);
                void registerEvent(AbstractEventProvider* eventProvider);
                void notifyEvent(const std::string& name);
                void notifyAll();
                void addListener(const std::string& name, IEventListener* listener);
                void addListener(const std::string& name, IEventListenerProvider* listener);
                void removeListener(const std::string& name, IEventListener* listener);
                bool hasEvent(const std::string& name) const;
                chimera::simulation::Observer* getObserver(const std::string& name);
            protected:
            private:
                std::unordered_map<std::string, size_t>* _observerMap;
                std::vector<DefaultEventProvider*>* _defaultProvider;
                std::vector<AbstractEventProvider*>* _provider;
                std::vector<Observer*>* _observer;
        };
    }
}

#endif // NOTIFICATIONMANAGER_H
