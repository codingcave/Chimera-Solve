#ifndef OUTPUTENTRYPOINT_H
#define OUTPUTENTRYPOINT_H

class OutputEntryPoint:
    public EntryPoint
{
    public:
        OutputEntryPoint();
        virtual ~OutputEntryPoint();
        virtual const std::string getName() const override;
        virtual const std::string getVersion() const override;
    protected:
        virtual bool onLoad(Registry const * const reg) override;
        virtual void init(lua_State* L) override;
    private:

};

#endif // OUTPUTENTRYPOINT_H
