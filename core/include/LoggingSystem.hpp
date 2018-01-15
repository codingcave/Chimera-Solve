#ifndef LOGGINGSYSTEM_H
#define LOGGINGSYSTEM_H

namespace chimera {
    typedef std::vector<ILogger*> vec_t_logList;

    class LoggingSystem
    {
        public:
            LoggingSystem();
            virtual ~LoggingSystem();
            void Log(const std::string& message);
            void Warning(const std::string& message);
            void Error(const std::string& message);
            void AddLogger(ILogger* logger);
            bool RemoveLogger(ILogger* logger);
        private:
            vec_t_logList *_logList;
    };
}

#endif // LOGGINGSYSTEM_H
