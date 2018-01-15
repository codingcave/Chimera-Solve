#ifndef INTEGRATORENTRYPOINT_H
#define INTEGRATORENTRYPOINT_H

namespace chimera {
    namespace runtime {

        class IntegratorEntryPoint:
            public chimera::EntryPoint
        {
            public:
                IntegratorEntryPoint(InitializerEntryPoint* initializer);
                virtual ~IntegratorEntryPoint();
                virtual const std::string getGUID() const override;
                virtual const std::string getVersion() const override;
            protected:
                virtual bool checkModule(chimera::Module const * const module) const override;
                virtual void init(lua_State* L) override;
                virtual void const * const getModuleLoadParams() const override;
            private:
                InitializerEntryPoint* _initializer;
        };
    }
}

#endif // INTEGRATORENTRYPOINT_H
