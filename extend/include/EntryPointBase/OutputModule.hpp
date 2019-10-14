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
                void* getInstance(chimera::vec_t_LuaItem& parameters) const final;
                virtual IEventListener* getOutputInstance(chimera::vec_t_LuaItem& parameters) const = 0;
                virtual size_t getFlag(const std::string& flag) const override;
            protected:
            private:
        };
    }
}

#endif // OUTPUTREGISTRY_H
