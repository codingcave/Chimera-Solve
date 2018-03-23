#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <limits>

#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "EntryPointBase/TemplateIntegrator.hpp"
#include "interfaces/ISimulation.hpp"
#include "EntryPointBase/ObserverModule.hpp"
#include "TimedObserver.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new TimedObserverModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (TimedObserverModule*)module;
    }
}


TimedObserverModule::TimedObserverModule()
{
    //ctor
}

TimedObserverModule::~TimedObserverModule()
{
    //dtor
}

const std::string TimedObserverModule::getGUID() const
{
    return "Timed Observer";
}

void TimedObserverModule::destroyInstance(void* instance) const
{
    delete ((TimedObserverProvider*)instance);
}

const std::string TimedObserverModule::getVersion() const
{
    return "1.0.0";
}

chimera::simulation::AbstractEventProvider* TimedObserverModule::getEventInstance(chimera::vec_t_LuaItem& parameters) const
{
    double start = 0.0;
    double step = 1.0;
    double end = std::numeric_limits<double>::infinity();

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "start" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                start = p.second;
            }
            if(p.first == "step" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                if(step <= 0)
                {
                    return nullptr;
                }
                step = p.second;
            }
            if(p.first == "end" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                end = p.second;
            }
        }
        return new TimedObserverProvider(start, step, end);
    }
    else
    {
        if(parameters.size() == 3)
        {
            if(chimera::systemtypes::PID_NUMBER == parameters[0].getType() &&
               chimera::systemtypes::PID_NUMBER == parameters[1].getType() &&
               chimera::systemtypes::PID_NUMBER == parameters[2].getType())
            {
                start = parameters[0];
                step = parameters[1];
                end = parameters[2];
            }
            else
            {
                return nullptr;
            }
        }
        else  if(parameters.size() == 2)
        {
            if(chimera::systemtypes::PID_NUMBER == parameters[0].getType() &&
               chimera::systemtypes::PID_NUMBER == parameters[1].getType())
            {
                start = parameters[0];
                step = parameters[1];
            }
            else
            {
                return nullptr;
            }
        }
        else  if(parameters.size() == 1)
        {
            if(chimera::systemtypes::PID_NUMBER == parameters[0].getType())
            {
                step = parameters[0];
            }
            else
            {
                return nullptr;
            }
        }
    }

    if(step <= 0)
    {
        return nullptr;
    }
    return new TimedObserverProvider(start, step, end);
}

TimedObserverProvider::TimedObserverProvider(double start, double step, double end):
    _start(start),
    _step(step),
    _end(end),
    _last(start - step)
{
    _args1 = new struct chimera::simulation::T_TimeStateArgs<double, boost::numeric::ublas::vector<double> >();
    _args2 = new struct chimera::simulation::T_TimeStateArgs<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >();
}

TimedObserverProvider::~TimedObserverProvider()
{
    delete _args1;
    delete _args2;
}

chimera::simulation::Observer* TimedObserverProvider::getObserver(chimera::simulation::NotificationManager* sender)
{
    chimera::simulation::ISimulation* sim = dynamic_cast<chimera::simulation::ISimulation*>(sender);
    if(sim)
    {
        {
            chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<double> >* integrator = dynamic_cast<chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<double> >*>(sim->getIntegrator());
            if(integrator)
            {
                return new chimera::simulation::TemplateStateObserver<double, boost::numeric::ublas::vector<double> >(integrator->getTimeType(), integrator->getStateType());
            }
        }
        {
            chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >* integrator = dynamic_cast<chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >*>(sim->getIntegrator());
            if(integrator)
            {
                return new chimera::simulation::TemplateStateObserver<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >(integrator->getTimeType(), integrator->getStateType());
            }
        }
    }
}

bool TimedObserverProvider::triggerCondition(chimera::simulation::NotificationManager* sender)
{
    chimera::simulation::ISimulation* sim = dynamic_cast<chimera::simulation::ISimulation*>(sender);
    if(sim)
    {
        chimera::simulation::AbstractIntegrator* integrator = dynamic_cast<chimera::simulation::AbstractIntegrator*>(sim->getIntegrator());
        if(integrator)
        {
            double time = *((double*)integrator->currentTime());
            if(time >= _last + _step)
            {
                while(_last + _step < time)
                {
                    _last += _step;
                }
                return true;
            }
        }
    }
    return false;
}

void* TimedObserverProvider::getEventArgs(chimera::simulation::NotificationManager* sender)
{
    chimera::simulation::ISimulation* sim = dynamic_cast<chimera::simulation::ISimulation*>(sender);
    if(sim)
    {
        chimera::simulation::AbstractIntegrator* integrator = dynamic_cast<chimera::simulation::AbstractIntegrator*>(sim->getIntegrator());
        if(integrator)
        {

        }

        {
            chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<double> >* integrator = dynamic_cast<chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<double> >*>(sim->getIntegrator());
            if(integrator)
            {
                _args1->time = integrator->getTime();
                _args1->state = integrator->getState();
                return _args1;
            }
        }
        {
            chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >* integrator = dynamic_cast<chimera::simulation::TemplateIntegrator<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >*>(sim->getIntegrator());
            if(integrator)
            {
                _args2->time = integrator->getTime();
                _args2->state = integrator->getState();
                return _args2;
            }
        }
    }
}

std::string TimedObserverProvider::getName() const
{
    return "elapsed";
}
