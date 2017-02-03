#ifndef ABSTRACTINTEGRATOR_H
#define ABSTRACTINTEGRATOR_H

class AbstractIntegrator
{
    friend class Simulation;
    public:
        AbstractIntegrator();
        virtual ~AbstractIntegrator();

        virtual size_t getTimeType() const = 0;
        virtual size_t getStateType() const = 0;
        virtual void * currentTime() = 0;
        virtual void * currentState() = 0;
        virtual bool nextStep() = 0;
        virtual void initialize(vec_t_LuaItem args) = 0;
        virtual void start(vec_t_LuaItem args) = 0;
        const NotificationManager* getNotificationManager();
    protected:
    private:
        NotificationManager* _manager;
};

#endif // ABSTRACTINTEGRATOR_H
