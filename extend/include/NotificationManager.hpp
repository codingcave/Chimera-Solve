#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

typedef void (*fn_t_listener)(void const * const sender, void* args);

class NotificationManager
{
    public:
        NotificationManager();
        virtual ~NotificationManager();
        void registerEvent(const std::string& name, void* sender);
        void notifyEvent(const std::string& name, void* sender, void* args);
        void addListener(const std::string& name, fn_t_listener listener);
        void removeListener(const std::string& name, fn_t_listener listener);
        void addListener(const std::string& name, IEventListener* listener);
        void removeListener(const std::string& name, IEventListener* listener);
        bool hasEvent(const std::string& name) const;
    protected:
    private:
        std::unordered_map<std::string, void*>* _eventOwner;
        std::unordered_map<std::string, std::list<fn_t_listener> >* _fn_listener;
        std::unordered_map<std::string, std::list<IEventListener*> >* _i_listener;
};

#endif // NOTIFICATIONMANAGER_H
