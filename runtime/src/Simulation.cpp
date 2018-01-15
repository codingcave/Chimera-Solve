#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>

#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "Simulation.hpp"

chimera::runtime::Simulation::Simulation(chimera::simulation::AbstractIntegrator* integrator)
{
    _running = false;
    _integrator = integrator;
}

chimera::runtime::Simulation::~Simulation()
{
    //dtor
}

bool chimera::runtime::Simulation::isRunning() const
{
    return _running;
}

bool chimera::runtime::Simulation::getYield() const
{
    return _yield;
}

void chimera::runtime::Simulation::setYield(bool value)
{
    _yield = value;
    if(value)
        _running = false;
}

bool chimera::runtime::Simulation::nextStep()
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

chimera::simulation::AbstractIntegrator * chimera::runtime::Simulation::getIntegrator()
{
    return _integrator;
}
