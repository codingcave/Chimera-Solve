#ifndef STANDARDLOGGER_H
#define STANDARDLOGGER_H

#include "LoggingSystem.hpp"

class StandardLogger:public LoggingSystem
{
    public:
        StandardLogger();
        virtual ~StandardLogger();
    protected:
        void CreateLog(const std::string& message) override;
        void CreateWarning(const std::string& message) override;
        void CreateError(const std::string& message) override;
    private:
};

#endif // STANDARDLOGGER_H
