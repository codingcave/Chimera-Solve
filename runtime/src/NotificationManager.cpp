#include <iostream>
#include <unordered_map>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"

chimera::runtime::NotificationManager::NotificationManager()
{
    _listener = new std::unordered_map<std::string, chimera::simulation::EventManager*>();
}

chimera::runtime::NotificationManager::~NotificationManager()
{
    delete _listener;
}

void chimera::runtime::NotificationManager::registerEvent(const std::string& name, chimera::simulation::EventManager* evList)
{
    if(!hasEvent(name))
    {
        (*_listener)[name] = evList;
    }
}

void chimera::runtime::NotificationManager::notifyEvent(const std::string& name, void* sender, void* args)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        if(available->second->getOwner() == nullptr || available->second->getOwner() == sender)
        {
            available->second->trigger(sender, args);
        }
    }
}

void chimera::runtime::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        available->second->addListener(listener);
    }
}

void chimera::runtime::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListenerProvider* listener)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        available->second->addListener(listener);
    }
}

void chimera::runtime::NotificationManager::removeListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        available->second->removeListener(listener);
    }
}

bool chimera::runtime::NotificationManager::hasEvent(const std::string& name) const
{
    return _listener->count(name);
}

chimera::simulation::EventManager* chimera::runtime::NotificationManager::getEventManager(const std::string& name)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        return available->second;
    }
    return nullptr;
}
