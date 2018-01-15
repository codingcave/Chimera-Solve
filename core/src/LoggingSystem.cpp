#include <iostream>
#include <vector>

#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"

chimera::LoggingSystem::LoggingSystem()
{
    _logList = new vec_t_logList();
}

chimera::LoggingSystem::~LoggingSystem()
{
    delete _logList;
}

void chimera::LoggingSystem::Log(const std::string& message)
{
    for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        (*it)->CreateLog(message);
}

void chimera::LoggingSystem::Warning(const std::string& message)
{
    for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        (*it)->CreateWarning(message);
}

void chimera::LoggingSystem::Error(const std::string& message)
{
    for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        (*it)->CreateError(message);
    // exit
}

void chimera::LoggingSystem::AddLogger(chimera::ILogger *logger)
{
    for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        if(*it == logger) return;
    _logList->push_back(logger);
}

bool chimera::LoggingSystem::RemoveLogger(chimera::ILogger *logger)
{
    for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
    {
        if(*it == logger)
        {
            _logList->erase(it);
            return true;
        }
    }
    return false;
}
