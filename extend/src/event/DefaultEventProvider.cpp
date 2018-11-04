#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "event/Observer.hpp"
//#include "interfaces/IEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::DefaultEventProvider::DefaultEventProvider()
{
    //ctor
}

chimera::simulation::DefaultEventProvider::~DefaultEventProvider()
{
    //dtor
}

bool chimera::simulation::DefaultEventProvider::triggerCondition(NotificationManager const * const)
{
    return true;
}

void* chimera::simulation::DefaultEventProvider::getEventArgs(NotificationManager const * const)
{
    return nullptr;
}
