#ifndef CSV_FILE_H
#define CSV_FILE_H

class CsvFileRegistry:
    public OutputMultiRegistry
{
    public:
        CsvFileRegistry();
        virtual ~CsvFileRegistry();
        virtual void destroyInstance(void * const instance) const override;
        virtual IEventListenerProvider* getOutputInstance(vec_t_LuaItem& parameters) const override;
        virtual const std::string getVersion() const override;
    protected:
    private:
};

class CsvFileWriter:
    public IEventListenerProvider
{
    public:
        CsvFileWriter(const std::string& path);
        virtual ~CsvFileWriter();
        void open();
        void close();
        const std::string getPath() const;
        virtual IEventListener* provideListener(size_t id, void* args) override;
        //virtual void notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state) override;
    protected:
    private:
        std::ofstream* _file;
        const std::string _path;
};

class FileOutput_real_vecReal:
    public StateEventListener<double, boost::numeric::ublas::vector<double> >
{
    public:
        FileOutput_real_vecReal(std::ofstream* file);
        virtual ~FileOutput_real_vecReal();
        virtual void notify(const double& time, const boost::numeric::ublas::vector<double>& state) override;
        virtual void notify(void const * const sender, void* args) override;
    private:
        std::ofstream* _file;
};

#endif // CSV_FILE_H
