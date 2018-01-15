#ifndef COUPLINGENTRYPOINT_H
#define COUPLINGENTRYPOINT_H


class CouplingEntryPoint:
    public chimera::EntryPoint
{
    public:
        CouplingEntryPoint();
        virtual ~CouplingEntryPoint();
        virtual const std::string getGUID() const override;
        virtual const std::string getVersion() const override;
    protected:
        virtual bool checkModule(chimera::Module const * const module) const override;
        virtual void init(lua_State* L) override;
    private:
};


#endif // COUPLINGENTRYPOINT_H
