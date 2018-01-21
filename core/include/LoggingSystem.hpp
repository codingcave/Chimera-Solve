#ifndef LOGGINGSYSTEM_H
#define LOGGINGSYSTEM_H

namespace chimera {
    typedef std::map<unsigned int, std::vector<chimera::ILogger*> > vec_t_logList;

    class LoggingSystem
    {
        friend class ChimeraSystem;
        public:
            LoggingSystem();
            virtual ~LoggingSystem();
            void log(const unsigned int& level, const std::string& message);
            void debug(const std::string& message);
            void addLogger(const unsigned int& level, chimera::ILogger* logger);
            bool removeLogger(chimera::ILogger* logger);
        private:
            vec_t_logList* _logList;
            unsigned int _loglevel;
            void setLogLevel(const unsigned int& loglevel);
    };
}

#endif // LOGGINGSYSTEM_H
