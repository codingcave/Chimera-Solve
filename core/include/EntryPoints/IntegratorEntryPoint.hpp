#ifndef INTEGRATORENTRYPOINT_H
#define INTEGRATORENTRYPOINT_H


class IntegratorEntryPoint:
    public EntryPoint
{
    public:
        IntegratorEntryPoint();
        virtual ~IntegratorEntryPoint();
        virtual const std::string getName() const override;
        virtual const std::string getVersion() const override;
    protected:
        virtual bool onLoad(Registry const * const reg) override;
        virtual void init(lua_State* L) override;
    private:
};

#endif // INTEGRATORENTRYPOINT_H
