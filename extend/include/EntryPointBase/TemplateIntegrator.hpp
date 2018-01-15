#ifndef TEMPLATEINTEGRATOR_H
#define TEMPLATEINTEGRATOR_H

namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;

        template<typename time_type, typename state_type>
        class TemplateStateEventmanager;

        template<typename time_type, typename state_type>
        class TemplateIntegrator:
            public AbstractIntegrator
        {
            public:
                TemplateIntegrator(){}
                virtual ~TemplateIntegrator(){}

                //void * currentTime() override { return (void *)getTime(); }
                //void * currentState() override { return (void *)getState(); }
                virtual const time_type& getTime() const = 0;
                virtual const state_type& getState() const = 0;

                virtual EventManager* createStateEventManager() override
                {
                    return new TemplateStateEventmanager<time_type, state_type>(this);
                }
            protected:
            private:
        };

        template<typename time_type, typename state_type>
        class TemplateStateEventmanager:
            public EventManager
        {
            public:
                TemplateStateEventmanager(TemplateIntegrator<time_type, state_type>* integrator){
                    _integrator = integrator;
                    _listeners = new std::unordered_set<StateEventListener<time_type, state_type>* >();
                }
                virtual ~TemplateStateEventmanager(){
                    delete _listeners;
                }

                virtual void trigger(void* sender, void* args) override
                {
                    for(auto it : *_listeners) {
                        it->notify(_integrator->getTime(), _integrator->getState());
                    }
                }
                virtual void addListener(IEventListener* listener) override
                {
                    StateEventListener<time_type, state_type>* l = dynamic_cast<StateEventListener<time_type, state_type>* >(listener);
                    if(l)
                    {
                        _listeners->insert(l);
                    }
                }
                virtual void addListener(IEventListenerProvider* provider) override
                {
                    struct StateProviderArgs args = {_integrator->getTimeType(), _integrator->getStateType()};
                    IEventListener* listener = provider->provideListener(1, &args);
                    StateEventListener<time_type, state_type>* l = dynamic_cast<StateEventListener<time_type, state_type>* >(listener);
                    if(l)
                    {
                        _listeners->insert(l);
                    }
                }
                virtual void removeListener(IEventListener* listener) override
                {
                    StateEventListener<time_type, state_type>* l = dynamic_cast<StateEventListener<time_type, state_type>* >(listener);
                    if(l)
                    {
                        _listeners->erase(l);
                    }
                }
            protected:
            private:
                TemplateIntegrator<time_type, state_type>* _integrator;
                std::unordered_set<StateEventListener<time_type, state_type>* >* _listeners;
        };
    }
}
#endif // TEMPLATEINTEGRATOR_H
