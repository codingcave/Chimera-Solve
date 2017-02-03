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
#include "Simulation.hpp"

Simulation::Simulation(AbstractIntegrator* integrator)
{
    _running = false;
    integrator->_manager = this;
    _integrator = integrator;
}

Simulation::~Simulation()
{
    //dtor
}

bool Simulation::isRunning() const
{
    return _running;
}

bool Simulation::getYield() const
{
    return _yield;
}

void Simulation::setYield(bool value)
{
    _yield = value;
    if(value)
        _running = false;
}

bool Simulation::nextStep()
{
    _running = true;
    if(_integrator->nextStep())
    {
        return true;
    }
    else
    {
        _running = false;
        return false;
    }
}

AbstractIntegrator * Simulation::getIntegrator()
{
    return _integrator;
}
