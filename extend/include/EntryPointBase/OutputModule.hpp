#ifndef OUTPUTREGISTRY_H
#define OUTPUTREGISTRY_H

namespace chimera {
    namespace simulation {
        class OutputModule:
            public chimera::Module
        {
            public:
                OutputModule();
                virtual ~OutputModule();
                void* getInstance(chimera::vec_t_LuaItem& parameters) const override;
                virtual IEventListener* getOutputInstance(chimera::vec_t_LuaItem& parameters) const = 0;
            protected:
            private:
        };
    }
}

#endif // OUTPUTREGISTRY_H
