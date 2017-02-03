#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H


class EventManager
{
    public:
        EventManager();
        EventManager(void* owner);
        virtual ~EventManager();
        virtual void notifyEvent(void* sender, void* args) = 0;
        virtual void addListener(IEventListener* listener) = 0;
        virtual void removeListener(IEventListener* listener) = 0;
        virtual void const * const getOwner() const;
    protected:
        void* _owner;
    private:
};

#endif // EVENTMANAGER_H
