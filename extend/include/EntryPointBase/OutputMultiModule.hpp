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
                void* getInstance(chimera::vec_t_LuaItem& parameters) const final;
                virtual IEventListenerProvider* getOutputInstance(chimera::vec_t_LuaItem& parameters) const = 0;
                virtual size_t getFlag(const std::string& flag) const override;
            protected:
            private:
        };
    }
}

#endif // OUTPUTMULTIREGISTRY_H
