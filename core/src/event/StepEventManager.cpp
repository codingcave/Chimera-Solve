#include <iostream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "event/StepEventListener.hpp"
#include "event/EventManager.hpp"
#include "event/StepEventManager.hpp"

StepEventManager::StepEventManager()
{
    _listeners = nullptr;
}

StepEventManager::~StepEventManager()
{
    if(_listeners)
        delete _listeners;
}

void StepEventManager::notifyEvent(void* sender, void* args)
{
    std::tuple<size_t, void*, size_t, void*>* t = (std::tuple<size_t, void*, size_t, void*>*)args;
    notifyStep(sender, std::get<0>(*t), std::get<1>(*t), std::get<2>(*t), std::get<3>(*t));
}

void StepEventManager::notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state)
{
    if(_listeners)
    {
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            (*it)->notifyStep(sender, timeType, time, stateType, state);
        }
    }
}

void StepEventManager::addListener(IEventListener* listener)
{
    StepEventListener* l = dynamic_cast<StepEventListener*>(listener);
    if(l)
    {
        if(!_listeners)
            _listeners = new std::list<StepEventListener*>();
        _listeners->push_back(l);
    }
}

void StepEventManager::removeListener(IEventListener* listener)
{
    StepEventListener* l = dynamic_cast<StepEventListener*>(listener);
    if(l)
    {
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(*it == l)
            {
                _listeners->erase(it);
                break;
            }
        }
    }
}
