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
#include "Euler/Euler_recursive_vecvecDouble.hpp"

namespace ublas = boost::numeric::ublas;

Euler_recursive_vecvecDouble::Euler_recursive_vecvecDouble(
        chimera::ParameterTypeSystem* ps,
        chimera::EntryPoint* init,
        chimera::simulation::TemplateOdeSystem<double, vec_vec_real > * system
    ):
    _ps(ps),
    _init((size_t)init)
{
    _system = system;
    _time = 0;
    int length = system->getFeatures()[chimera::simulation::Naming::Feature_size];
    size_t unit_length = system->getFeatures()[chimera::simulation::Naming::Feature_unit_size];
    _state = new vec_vec_real(length);
    _swapState = new vec_vec_real(length);
    for(unsigned int i = 0; i < length; i++)
    {
        (*_state)[i].resize(unit_length);
        (*_swapState)[i].resize(unit_length);
    }
}

Euler_recursive_vecvecDouble::~Euler_recursive_vecvecDouble()
{
    delete _state;
    delete _swapState;
}

size_t Euler_recursive_vecvecDouble::getTimeType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

size_t Euler_recursive_vecvecDouble::getStateType() const
{
    const std::string vectorVectorRealMetaName = std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER);
    return _ps->getParameterID(vectorVectorRealMetaName);
}

//double * RungeKutta::getTime()
void * Euler_recursive_vecvecDouble::currentTime()
{
    return new double(_time);
}

//struct T_VectorDef* RungeKutta::getState()
void * Euler_recursive_vecvecDouble::currentState()
{
    return new struct chimera::simulation::T_VectorDef({_state->size(), true, false, _state});
}

const double& Euler_recursive_vecvecDouble::getTime() const
{
    return _time;
}

const vec_vec_real& Euler_recursive_vecvecDouble::getState() const
{
    return *_state;
}

void Euler_recursive_vecvecDouble::nextStep()
{
    static vec_vec_real* tmpState;
    _system->operator()(*_state, *_swapState, _time);
    tmpState = _swapState;
    _swapState = _state;
    _state = tmpState;
    _time += 1.0;
}

void Euler_recursive_vecvecDouble::initialize(chimera::vec_t_LuaItem args)
{
    if(args.size() > 1) {
        if(args[0].getType() == chimera::systemtypes::PID_NUMBER)
        {
            _time = *((double*)args[0].getValue());
            if(_ps->getParameterBase(args[1].getType()) == chimera::systemtypes::PID_INSTANCE && _ps->getParameterTag(args[1].getType()) == _init)
            {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[1].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system, _state);
            }
        }
    } else {
        if(args.size() == 1)
        {
            if(_ps->getParameterBase(args[0].getType()) == chimera::systemtypes::PID_INSTANCE && _ps->getParameterTag(args[0].getType()) == _init)
            {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[0].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system, _state);
            }
        }
    }
}

void Euler_recursive_vecvecDouble::start(chimera::vec_t_LuaItem args)
{
    if(args.size() > 0 && args[0].getType() == chimera::systemtypes::PID_NUMBER) {
        _endtime = _time + (int)(*((double*)args[0].getValue()));
    } else {
        _endtime = _time + 1.0;
    }
}

bool Euler_recursive_vecvecDouble::finished() const
{
    return (int)_time >= (int)_endtime;
}
