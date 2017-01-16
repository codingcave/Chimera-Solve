#include <iostream>

#include "Naming.hpp"
#include "LoggingSystem.hpp"
#include "StandardLogger.hpp"

StandardLogger::StandardLogger()
{

}

StandardLogger::~StandardLogger()
{

}

void StandardLogger::CreateLog(const std::string& message)
{
    std::cout << Naming::Logging_log << message << std::endl;
}

void StandardLogger::CreateWarning(const std::string& message)
{
    std::cout << Naming::Logging_warning << message << std::endl;
}

void StandardLogger::CreateError(const std::string& message)
{
    std::cout << Naming::Logging_error << message << std::endl;
}
