#ifndef LOGGINGSYSTEM_H
#define LOGGINGSYSTEM_H

#include <vector>

class LoggingSystem;

typedef std::vector<LoggingSystem*> vec_t_logList;

class LoggingSystem
{
    public:
        LoggingSystem();
        virtual ~LoggingSystem();
        static void Log(const std::string& message);
        static void Warning(const std::string& message);
        static void Error(const std::string& message);
        static void AddLogger(LoggingSystem* logSys);
        static bool RemoveLogger(LoggingSystem* logSys);
        static void Init();
        static void ShutDown();
    protected:
        virtual void CreateLog(const std::string& message) = 0;
        virtual void CreateWarning(const std::string& message) = 0;
        virtual void CreateError(const std::string& message) = 0;
    private:
        static bool _init;
        static vec_t_logList *_logList;
};

#endif // LOGGINGSYSTEM_H
