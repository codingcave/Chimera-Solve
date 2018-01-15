#ifndef CSV_FILE_H
#define CSV_FILE_H

class CsvFileModule:
    public chimera::simulation::OutputMultiModule
{
    public:
        CsvFileModule();
        virtual ~CsvFileModule();
        virtual void destroyInstance(void * const instance) const override;
        virtual chimera::simulation::IEventListenerProvider* getOutputInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual const std::string getVersion() const override;
        virtual const std::string getGUID() const override;
    protected:
    private:
};

class CsvFileWriter:
    public chimera::simulation::IEventListenerProvider
{
    public:
        CsvFileWriter(chimera::ParameterTypeSystem* ps, const std::string& path);
        virtual ~CsvFileWriter();
        void open();
        void close();
        const std::string getPath() const;
        virtual chimera::simulation::IEventListener* provideListener(size_t id, void* args) override;
        //virtual void notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state) override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        std::ofstream* _file;
        const std::string _path;

};

class FileOutput_real_vecReal:
    public chimera::simulation::StateEventListener<double, boost::numeric::ublas::vector<double> >
{
    public:
        FileOutput_real_vecReal(std::ofstream* file);
        virtual ~FileOutput_real_vecReal();
        virtual void notify(const double& time, const boost::numeric::ublas::vector<double>& state) override;
        virtual void notify(void const * const sender, void* args) override;
    private:
        std::ofstream* _file;
};

class FileOutput_real_vecvecReal:
    public chimera::simulation::StateEventListener<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >
{
    public:
        FileOutput_real_vecvecReal(std::ofstream* file);
        virtual ~FileOutput_real_vecvecReal();
        virtual void notify(const double& time, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& state) override;
        virtual void notify(void const * const sender, void* args) override;
    private:
        std::ofstream* _file;
};

#endif // CSV_FILE_H
