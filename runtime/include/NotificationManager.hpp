#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

namespace chimera {
    namespace runtime {
        class IEventListener;
        class IEventListenerProvider;

        class NotificationManager
        {
            public:
                NotificationManager();
                virtual ~NotificationManager();
                void registerEvent(const std::string& name, chimera::simulation::EventManager* evList);
                void notifyEvent(const std::string& name, void* sender, void* args);
                void addListener(const std::string& name, chimera::simulation::IEventListener* listener);
                void addListener(const std::string& name, chimera::simulation::IEventListenerProvider* listener);
                void removeListener(const std::string& name, chimera::simulation::IEventListener* listener);
                bool hasEvent(const std::string& name) const;
                chimera::simulation::EventManager* getEventManager(const std::string& name);
            protected:
            private:
                std::unordered_map<std::string, chimera::simulation::EventManager*>* _listener;
        };
    }
}

#endif // NOTIFICATIONMANAGER_H
