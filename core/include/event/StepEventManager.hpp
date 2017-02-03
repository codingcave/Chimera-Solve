#ifndef STEPEVENTHANDLER_H
#define STEPEVENTHANDLER_H


class StepEventManager : public EventManager
{
    public:
        StepEventManager();
        virtual ~StepEventManager();
        virtual void notifyEvent(void* sender, void* args) override;
        void notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state);
        virtual void addListener(IEventListener* listener) override;
        virtual void removeListener(IEventListener* listener) override;
    protected:
    private:
        std::list<StepEventListener*>* _listeners;
};

#endif // STEPEVENTHANDLER_H
