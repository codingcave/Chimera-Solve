#include <iostream>
#include <vector>

#include "LoggingSystem.hpp"

bool LoggingSystem::_init;
vec_t_logList* LoggingSystem::_logList;

LoggingSystem::LoggingSystem()
{

}

LoggingSystem::~LoggingSystem()
{

}

void LoggingSystem::Log(const std::string& message)
{
    if(_init)
    {
        for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
            (*it)->CreateLog(message);
    }
}

void LoggingSystem::Warning(const std::string& message)
{
    if(_init)
    {
        for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
            (*it)->CreateWarning(message);
    }
}

void LoggingSystem::Error(const std::string& message)
{
    if(_init)
    {
        for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
            (*it)->CreateError(message);
    }
    // exit
}

void LoggingSystem::AddLogger(LoggingSystem *logSys)
{
    Init();
    for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        if(*it == logSys) return;
    _logList->push_back(logSys);
}

bool LoggingSystem::RemoveLogger(LoggingSystem *logSys)
{
    if(_init)
    {
        for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        {
            if(*it == logSys)
            {
                _logList->erase(it);
                return true;
            }
        }
    }
    return false;
}

void LoggingSystem::Init()
{
    if(!_init)
    {
        _init = true;
        _logList = new vec_t_logList();
    }
}

void LoggingSystem::ShutDown()
{
    if(_init)
    {
        //for (vec_t_logList::iterator it = _logList->begin() ; it != _logList->end(); ++it)
        //    delete *it;
        delete _logList;
        _init = false;
    }
}
