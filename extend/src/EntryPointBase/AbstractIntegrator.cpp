#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>

#include "ParameterType.hpp"
#include "def.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"

AbstractIntegrator::AbstractIntegrator()
{
    _manager = nullptr;
}


AbstractIntegrator::~AbstractIntegrator()
{
    //dtor
}

const NotificationManager* AbstractIntegrator::getNotificationManager()
{
    return _manager;
}
