#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::NotificationManager::NotificationManager()
{
    _observerMap = new std::unordered_map<std::string, size_t>();
    _defaultProvider = new std::vector<DefaultEventProvider*>();
    _provider = new std::vector<AbstractEventProvider*>();
    _observer = new std::vector<Observer*>();
}

chimera::simulation::NotificationManager::~NotificationManager()
{
    delete _observerMap;
    for(DefaultEventProvider* provider : (*_defaultProvider))
    {
        delete provider;
    }
    delete _defaultProvider;
    delete _observer;
    delete _provider;
}

void chimera::simulation::NotificationManager::registerEvent(const std::string& name, chimera::simulation::Observer* observer)
{
    if(!hasEvent(name))
    {
        DefaultEventProvider* p = new DefaultEventProvider(name, observer);
        _defaultProvider->push_back(p);
        size_t index = _provider->size();
        _provider->push_back(p);
        _observer->push_back(observer);
        _observerMap->insert(std::make_pair(name, index));
    }
}

void chimera::simulation::NotificationManager::registerEvent(AbstractEventProvider* eventProvider)
{
    size_t index = _provider->size();
    _provider->push_back(eventProvider);
    _observer->push_back(eventProvider->getObserver(this));
    _observerMap->insert(std::make_pair(eventProvider->getName(), index));
}

void chimera::simulation::NotificationManager::notifyEvent(const std::string& name)
{
    auto available = _observerMap->find (name);
    if(available != _observerMap->end())
    {
        if(!(*_observer)[available->second]->empty() && (*_provider)[available->second]->triggerCondition(this))
        {
            (*_observer)[available->second]->trigger(this, (*_provider)[available->second]->getEventArgs(this));
        }
    }
}

void chimera::simulation::NotificationManager::notifyAll()
{
    for(size_t i = 0; i < _provider->size(); i++)
    {
        if(!(*_observer)[i]->empty() && (*_provider)[i]->triggerCondition(this))
        {
            (*_observer)[i]->trigger(this, (*_provider)[i]->getEventArgs(this));
        }
    }
}

void chimera::simulation::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _observerMap->find (name);
    if(available != _observerMap->end())
    {
        (*_observer)[available->second]->addListener(listener);
    }
}

void chimera::simulation::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListenerProvider* listener)
{
    auto available = _observerMap->find (name);
    if(available != _observerMap->end())
    {
        (*_observer)[available->second]->addListener(listener);
    }
}

void chimera::simulation::NotificationManager::removeListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _observerMap->find (name);
    if(available != _observerMap->end())
    {
        (*_observer)[available->second]->removeListener(listener);
    }
}

bool chimera::simulation::NotificationManager::hasEvent(const std::string& name) const
{
    return _observerMap->count(name);
}

chimera::simulation::Observer* chimera::simulation::NotificationManager::getObserver(const std::string& name)
{
    auto available = _observerMap->find (name);
    if(available != _observerMap->end())
    {
        return (*_observer)[available->second];
    }
    return nullptr;
}
