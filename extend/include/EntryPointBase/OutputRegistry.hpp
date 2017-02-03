#ifndef OUTPUTREGISTRY_H
#define OUTPUTREGISTRY_H

class OutputRegistry:
    public Registry
{
    public:
        OutputRegistry();
        virtual ~OutputRegistry();
        void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual IEventListener* getOutputInstance(vec_t_LuaItem& parameters) const = 0;
        inline const std::string getEntrypoint() const override { return Naming::EntryPoint_output; }
    protected:
    private:
};

#endif // OUTPUTREGISTRY_H
