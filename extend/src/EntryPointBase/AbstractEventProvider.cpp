#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::AbstractEventProvider::AbstractEventProvider()
{
    _observer = new Observer();
}

chimera::simulation::AbstractEventProvider::~AbstractEventProvider()
{
    delete _observer;
}

void chimera::simulation::AbstractEventProvider::trigger(NotificationManager* sender)
{
    if(!_observer->empty() && triggerCondition(sender))
    {
        _observer->trigger(sender, getEventArgs(sender));
    }
}

chimera::simulation::Observer* chimera::simulation::AbstractEventProvider::getObserver()
{
    return _observer;
}
