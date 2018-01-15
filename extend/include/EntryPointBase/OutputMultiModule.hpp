#ifndef OUTPUTMULTIREGISTRY_H
#define OUTPUTMULTIREGISTRY_H

namespace chimera {
    namespace simulation {
        class OutputMultiModule:
            public Module
        {
            public:
                OutputMultiModule();
                virtual ~OutputMultiModule();
                void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
                virtual IEventListenerProvider* getOutputInstance(chimera::vec_t_LuaItem& parameters) const = 0;
            protected:
            private:
        };
    }
}

#endif // OUTPUTMULTIREGISTRY_H
