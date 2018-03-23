#ifndef OBSERVERMODULE_H
#define OBSERVERMODULE_H

namespace chimera {
    namespace simulation {
        class ObserverModule:
            public chimera::Module
        {
            public:
                ObserverModule();
                virtual ~ObserverModule();
                void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
                virtual AbstractEventProvider* getEventInstance(chimera::vec_t_LuaItem& parameters) const = 0;
            protected:
            private:
        };
    }
}

#endif // OBSERVERMODULE_H
