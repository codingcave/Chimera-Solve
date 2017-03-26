#include <iostream>
#include <unordered_map>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"

NotificationManager::NotificationManager()
{
    _listener = new std::unordered_map<std::string, EventManager*>();
}

NotificationManager::~NotificationManager()
{
    delete _listener;
}

void NotificationManager::registerEvent(const std::string& name, EventManager* evList)
{
    if(!hasEvent(name))
    {
        (*_listener)[name] = evList;
    }
}

void NotificationManager::notifyEvent(const std::string& name, void* sender, void* args)
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

void NotificationManager::addListener(const std::string& name, IEventListener* listener)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        available->second->addListener(listener);
    }
}

void NotificationManager::addListener(const std::string& name, IEventListenerProvider* listener)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        available->second->addListener(listener);
    }
}

void NotificationManager::removeListener(const std::string& name, IEventListener* listener)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        available->second->removeListener(listener);
    }
}

bool NotificationManager::hasEvent(const std::string& name) const
{
    return _listener->count(name);
}

EventManager* NotificationManager::getEventManager(const std::string& name)
{
    auto available = _listener->find (name);
    if(available != _listener->end())
    {
        return available->second;
    }
    return nullptr;
}
