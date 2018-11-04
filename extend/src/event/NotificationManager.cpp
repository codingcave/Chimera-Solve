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

chimera::simulation::NotificationManager::NotificationManager()
{
    _providerMap = new std::unordered_map<std::string, AbstractEventProvider*>();
}

chimera::simulation::NotificationManager::~NotificationManager()
{
    delete _providerMap;
}

void chimera::simulation::NotificationManager::registerEvent(const std::string& name, AbstractEventProvider* eventProvider)
{
    if(!hasEvent(name))
    {
        _providerMap->insert(std::make_pair(name, eventProvider));
    }
}

void chimera::simulation::NotificationManager::notifyEvent(const std::string& name)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        available->second->trigger(this);
    }
}

void chimera::simulation::NotificationManager::notifyAll()
{
    for(auto provider = _providerMap->begin(); provider != _providerMap->end(); provider++)
    {
        provider->second->trigger(this);
    }
}

void chimera::simulation::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        available->second->getObserver()->addListener(listener);
    }
}

chimera::simulation::IEventListener const * const chimera::simulation::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListenerProvider* provider)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        chimera::simulation::IEventListener* listener;
        listener = available->second->provideListener(provider);
        if (listener)
        {
            available->second->getObserver()->addListener(listener);
            return listener;
        }
    }

    return nullptr;
}

void chimera::simulation::NotificationManager::removeListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        available->second->getObserver()->removeListener(listener);
    }
}

bool chimera::simulation::NotificationManager::hasEvent(const std::string& name) const
{
    return _providerMap->count(name);
}

chimera::simulation::Observer* chimera::simulation::NotificationManager::getObserver(const std::string& name)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        return available->second->getObserver();
    }
    return nullptr;
}
