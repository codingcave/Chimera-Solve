#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::Observer::Observer()
{
    _listeners = new std::unordered_set<IEventListener*>;
}

chimera::simulation::Observer::~Observer()
{
    delete _listeners;
}


void chimera::simulation::Observer::trigger(NotificationManager* sender, void* args)
{
    for(auto it = _listeners->begin(); it != _listeners->end(); it++)
    {
        (*it)->notify(sender, args);
    }
}

void chimera::simulation::Observer::addListener(chimera::simulation::IEventListener* listener)
{
    if(!_listeners->count(listener)) {
        _listeners->insert(listener);
    }
}

void chimera::simulation::Observer::removeListener(chimera::simulation::IEventListener* listener)
{
    if(_listeners->count(listener)) {
        _listeners->erase(listener);
    }
}

bool chimera::simulation::Observer::empty() const
{
    return _listeners->size() == 0;
}
