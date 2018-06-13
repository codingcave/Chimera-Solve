#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>
//#include "lua.hpp"

#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/TemplateIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "Euler/Euler_recursive_vecDouble.hpp"

namespace ublas = boost::numeric::ublas;

Euler_recursive_vecDouble::Euler_recursive_vecDouble(
        chimera::ParameterTypeSystem* ps,
        chimera::EntryPoint* init,
        chimera::simulation::TemplateOdeSystem<double, ublas::vector<double> > * system
    ):
    _ps(ps),
    _init((size_t)init)
{
    _system = system;
    _time = 0;
    int length = system->getFeatures()[chimera::simulation::Naming::Feature_size];
    _state = new ublas::vector<double>(length);
    _swapState = new ublas::vector<double>(length);
}

Euler_recursive_vecDouble::~Euler_recursive_vecDouble()
{
    delete _state;
    delete _swapState;
}

size_t Euler_recursive_vecDouble::getTimeType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

size_t Euler_recursive_vecDouble::getStateType() const
{
    static const std::string vectorRealMetaName = std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER);
    return _ps->getParameterID(vectorRealMetaName);
}

//double * RungeKutta::getTime()
void * Euler_recursive_vecDouble::currentTime()
{
    return new double(_time);
}

//struct T_VectorDef* RungeKutta::getState()
void * Euler_recursive_vecDouble::currentState()
{
    return new struct chimera::simulation::T_VectorDef({_state->size(), true, false, _state});
}

const double& Euler_recursive_vecDouble::getTime() const
{
    return _time;
}

const boost::numeric::ublas::vector<double>& Euler_recursive_vecDouble::getState() const
{
    return *_state;
}

void Euler_recursive_vecDouble::nextStep()
{
    static boost::numeric::ublas::vector<double>* tmpState;
    _system->operator()(*_state, *_swapState, _time);
    tmpState = _swapState;
    _swapState = _state;
    _state = tmpState;
    _time += 1.0;
}

void Euler_recursive_vecDouble::initialize(chimera::vec_t_LuaItem args)
{
    if(args.size() > 1) {
        if(args[0].getType() == chimera::systemtypes::PID_NUMBER)
        {
            _time = *((double*)args[0].getValue());
            if(args[1].getType() == getStateType())
            {
                struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)args[1].getValue();
                ublas::vector<double>* newState = (ublas::vector<double>*)vd->value;
                if(newState->size() >= _state->size())
                {
                    for(unsigned int i = 0; i < _state->size(); i++)
                    {
                        (*_state)[i] = (*newState)[i];
                    }
                }
            }
            else if(args[1].getType() == chimera::systemtypes::PID_INSTANCE && _ps->getParameterTag(args[1].getType() == _init))
            {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[1].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system, _state);
            }
        }
    } else {
        if(args.size() == 1)
        {
            if(args[0].getType() == getStateType()) {
                struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)args[0].getValue();
                ublas::vector<double>* newState = (ublas::vector<double>*)vd->value;
                if(newState->size() >= _state->size())
                {
                    for(unsigned int i = 0; i < _state->size(); i++)
                    {
                        (*_state)[i] = (*newState)[i];
                    }
                }
            } else if(args[0].getType() == chimera::systemtypes::PID_INSTANCE && _ps->getParameterTag(args[0].getType() == _init)) {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[0].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system, _state);
            }
        }
    }
}

void Euler_recursive_vecDouble::start(chimera::vec_t_LuaItem args)
{
    if(args.size() > 0 && args[0].getType() == chimera::systemtypes::PID_NUMBER) {
        _endtime = _time + (int)(*((double*)args[0].getValue()));
    } else {
        _endtime = _time + 1.0;
    }
}

bool Euler_recursive_vecDouble::finished() const
{
    return (int)_time >= (int)_endtime;
}

