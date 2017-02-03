#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

class NotificationManager
{
    public:
        NotificationManager();
        virtual ~NotificationManager();
        void registerEvent(const std::string& name, EventManager* evList);
        void notifyEvent(const std::string& name, void* sender, void* args);
        void addListener(const std::string& name, IEventListener* listener);
        void removeListener(const std::string& name, IEventListener* listener);
        bool hasEvent(const std::string& name) const;
        EventManager* getEventManager(const std::string& name);
    protected:
    private:
        std::unordered_map<std::string, EventManager*>* _listener;
};

#endif // NOTIFICATIONMANAGER_H
