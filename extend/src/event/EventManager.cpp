#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"

EventManager::EventManager():
    _owner(nullptr)
{
    //ctor
}

EventManager::EventManager(void* owner):
    _owner(owner)
{

}

EventManager::~EventManager()
{
    //dtor
}

void const * const EventManager::getOwner() const
{
    return _owner;
}
