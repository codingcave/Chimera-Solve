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
                void* getInstance(chimera::vec_t_LuaItem& parameters) const final;
                virtual AbstractEventManager* getEventInstance(chimera::vec_t_LuaItem& parameters) const = 0;
                virtual size_t getFlag(const std::string& flag) const override;
            protected:
            private:
        };
    }
}

#endif // OBSERVERMODULE_H
