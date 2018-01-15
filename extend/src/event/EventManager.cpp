#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"

chimera::simulation::EventManager::EventManager():
    _owner(nullptr)
{
    //ctor
}

chimera::simulation::EventManager::EventManager(void* owner):
    _owner(owner)
{

}

chimera::simulation::EventManager::~EventManager()
{
    //dtor
}

void const * const chimera::simulation::EventManager::getOwner() const
{
    return _owner;
}
