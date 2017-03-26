#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

class IEventListener
{
    public:
        IEventListener() {}
        virtual ~IEventListener() {}
        virtual void notify(void const * const sender, void* args) = 0;
    protected:
    private:
};

#endif // IEVENTLISTENER_H
