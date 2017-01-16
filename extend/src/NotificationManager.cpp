#include <iostream>
#include <unordered_map>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "NotificationManager.hpp"

NotificationManager::NotificationManager()
{
    _eventOwner = new std::unordered_map<std::string, void*>();
    _fn_listener = new std::unordered_map<std::string, std::list<fn_t_listener> >();
    _i_listener = new std::unordered_map<std::string, std::list<IEventListener*> >();
}

NotificationManager::~NotificationManager()
{
    delete _eventOwner;
    delete _fn_listener;
    delete _i_listener;
}

void NotificationManager::registerEvent(const std::string& name, void* sender)
{
    if(!hasEvent(name))
    {
        if(sender)
        {
            (*_eventOwner)[name] = sender;
        }
        (*_fn_listener)[name] = std::list<fn_t_listener>();
        (*_i_listener)[name] = std::list<IEventListener*>();
    }
}

void NotificationManager::notifyEvent(const std::string& name, void* sender, void* args)
{
    auto available_fn = _fn_listener->find (name);
    if(available_fn != _fn_listener->end())
    {
        auto owner = _eventOwner->find (name);
        if(owner->second == nullptr || owner->second == sender)
        {
            for(auto it = available_fn->second.begin(); it != available_fn->second.end(); it++)
            {
                // TODO: test for nullptr
                // warning
                // remove nullptr
                (**it)(sender, args);
            }

            auto available_i = &((*_i_listener)[name]);
            for(auto it = available_i->begin(); it != available_i->end(); it++)
            {
                // TODO: test for nullptr
                // warning
                // remove nullptr
                (*it)->notify(sender, args);
            }
        }
    }
}

void NotificationManager::addListener(const std::string& name, fn_t_listener listener)
{
    auto available = _fn_listener->find (name);
    if(available != _fn_listener->end())
    {
        for(auto it = available->second.begin(); it != available->second.end(); it++)
        {
            if(*it == listener)
            {
                return;
            }
        }
        available->second.push_back(listener);
    }
}

void NotificationManager::removeListener(const std::string& name, fn_t_listener listener)
{
    auto available = _fn_listener->find (name);
    if(available != _fn_listener->end())
    {
        for(auto it = available->second.begin(); it != available->second.end(); it++)
        {
            if(*it == listener)
            {
                available->second.erase(it);
                break;
            }
        }
    }
}

void NotificationManager::addListener(const std::string& name, IEventListener* listener)
{
    auto available = _i_listener->find (name);
    if(available != _i_listener->end())
    {
        for(auto it = available->second.begin(); it != available->second.end(); it++)
        {
            if(*it == listener)
            {
                return;
            }
        }
        available->second.push_back(listener);
    }
}

void NotificationManager::removeListener(const std::string& name, IEventListener* listener)
{
    auto available = _i_listener->find (name);
    if(available != _i_listener->end())
    {
        for(auto it = available->second.begin(); it != available->second.end(); it++)
        {
            if(*it == listener)
            {
                available->second.erase(it);
                break;
            }
        }
    }
}

bool NotificationManager::hasEvent(const std::string& name) const
{
    return _fn_listener->count(name);
}
