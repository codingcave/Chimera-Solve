#ifndef OUTPUTMULTIREGISTRY_H
#define OUTPUTMULTIREGISTRY_H


class OutputMultiRegistry:
    public Registry
{
    public:
        OutputMultiRegistry();
        virtual ~OutputMultiRegistry();
        void* getInstance(vec_t_LuaItem& parameters) const override;
        virtual IEventListenerProvider* getOutputInstance(vec_t_LuaItem& parameters) const = 0;
        inline const std::string getEntrypoint() const override { return Naming::EntryPoint_output; }
    protected:
    private:
};

#endif // OUTPUTMULTIREGISTRY_H
