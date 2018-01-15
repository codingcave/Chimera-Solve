#ifndef ABSTRACTINTEGRATOR_H
#define ABSTRACTINTEGRATOR_H

namespace chimera {
    namespace simulation {
        class AbstractIntegrator
        {
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
                virtual EventManager* createStateEventManager() = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTINTEGRATOR_H
