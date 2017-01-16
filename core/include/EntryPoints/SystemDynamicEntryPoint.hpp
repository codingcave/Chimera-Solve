#ifndef SYSTEMDYNAMICENTRYPOINT_H
#define SYSTEMDYNAMICENTRYPOINT_H

class SystemDynamicEntryPoint:
    public EntryPoint
{
    public:
        SystemDynamicEntryPoint();
        virtual ~SystemDynamicEntryPoint();
        virtual const std::string getName() const override;
        virtual const std::string getVersion() const override;
    protected:
        virtual bool onLoad(Registry const * const reg) override;
        virtual void init(lua_State* L) override;
    private:
        const Registry* _active;
};

#endif // SYSTEMDYNAMICENTRYPOINT_H
