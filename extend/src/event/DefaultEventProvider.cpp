#include <iostream>
#include <unordered_map>
#include <vector>

#include "event/Observer.hpp"
//#include "interfaces/IEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::DefaultEventProvider::DefaultEventProvider(const std::string& name, Observer* observer):
    _name(name),
    _observer(observer)
{
    //ctor
}

chimera::simulation::DefaultEventProvider::~DefaultEventProvider()
{
    //dtor
}

chimera::simulation::Observer* chimera::simulation::DefaultEventProvider::getObserver(NotificationManager*)
{
    return _observer;
}

bool chimera::simulation::DefaultEventProvider::triggerCondition(NotificationManager*)
{
    return true;
}

void* chimera::simulation::DefaultEventProvider::getEventArgs(NotificationManager*)
{
    return nullptr;
}

std::string chimera::simulation::DefaultEventProvider::getName() const
{
    return _name;
}
