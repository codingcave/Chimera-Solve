#ifndef CSV_FILE_H
#define CSV_FILE_H


class CsvFileRegistry:
    public OutputRegistry
{
    public:
        CsvFileRegistry();
        virtual ~CsvFileRegistry();
        virtual void destroyInstance(void * const instance) const override;
        virtual IEventListener* getOutputInstance(vec_t_LuaItem& parameters) const override;
        virtual const std::string getVersion() const override;
    protected:
    private:
};

class CsvFileWriter:
    public StepEventListener
{
    public:
        CsvFileWriter(const std::string& path);
        virtual ~CsvFileWriter();
        void open();
        void close();
        const std::string getPath() const;
        virtual void notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state) override;
    protected:
    private:
        std::ofstream* _file;
        const std::string _path;
};

#endif // CSV_FILE_H
