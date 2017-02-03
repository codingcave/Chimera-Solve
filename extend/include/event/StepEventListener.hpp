#ifndef STEPEVENTLISTENER_H
#define STEPEVENTLISTENER_H


class StepEventListener:
    public IEventListener
{
    public:
        StepEventListener();
        virtual ~StepEventListener();
        virtual void notify(void const * const sender, void* args) override;
        virtual void notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state) = 0;
    protected:
    private:
};

#endif // STEPEVENTLISTENER_H
