#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>
//#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/TemplateIntegrator.hpp"
#include "EntryPointBase/IntegratorRegistry.hpp"
#include "RungeKutta.hpp"

namespace ublas = boost::numeric::ublas;


extern "C"
{
    Registry* LoadModule()
    {
        return new RungeKuttaRegistry();

    }
}

RungeKuttaRegistry::RungeKuttaRegistry()
{

}

RungeKuttaRegistry::~RungeKuttaRegistry()
{

}

//void* RungeKuttaRegistry::getInstance(vec_t_LuaItem& parameters) const
//{
//    return new RungeKutta(nullptr);
//}

const std::string RungeKuttaRegistry::getVersion() const
{
    return "1.0.0";
}


void RungeKuttaRegistry::destroyInstance(void * const instance) const
{
    delete (AbstractIntegrator*)instance;
}

AbstractIntegrator* RungeKuttaRegistry::getIntegratorInstance(vec_t_LuaItem& parameters) const
{
    static const std::string vectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    static const std::string vectorVectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    static const size_t vectorRealType = ParameterTypeSystem::getParameterID(vectorRealMetaName);
    static const size_t vectorVectorRealType = ParameterTypeSystem::getParameterID(vectorVectorRealMetaName);

    static const size_t basetype = ParameterTypeSystem::getParameterID(Naming::Type_Instance);
    static const size_t tagtype = ParameterTypeSystem::getParameterID(std::string(Naming::Lua_name_EntryPoint) + std::string("/") + std::string(Naming::EntryPoint_dynamics));

    if(parameters.size() > 0) {
        double dt = 0.1;
        if(parameters.size() > 1 && parameters[1].type == ParameterTypeSystem::pid_real)
        {
            dt = *((double*)parameters[1].value);
            if(dt <= 0.0) dt = .1;
        }

        if(ParameterTypeSystem::getParameterBase(parameters[0].type) == basetype && ParameterTypeSystem::getParameterTag(parameters[0].type) == tagtype)
        {
            AbstractSystemDynamic* dyn = (AbstractSystemDynamic*)parameters[0].value;
            auto features = dyn->getFeatures();

            auto timeType = features.find(Naming::Feature_time_type);
            auto stateType = features.find(Naming::Feature_state_type);
            if(timeType != features.end() && stateType != features.end()) {
                if(timeType->second == ParameterTypeSystem::pid_real) {
                    if(stateType->second == vectorRealType) {
                        TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* sys = dynamic_cast<TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(dyn);
                        if(sys != nullptr) {
                            return new RungeKutta_double_vecDouble(sys, dt);
                        }
                    }
                    if(stateType->second == vectorVectorRealType) {
                        TemplateOdeSystem<double, vec_vec_real>* sys = dynamic_cast<TemplateOdeSystem<double, vec_vec_real>*>(dyn);
                        if(sys != nullptr) {
                            //return new RungeKutta_double_vecvecDouble(sys, dt);
                            std::cout << "RETURN new RungeKutta_double_vecvecDouble(sys, dt);" << std::endl;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

bool RungeKuttaRegistry::checkFeatures(const map_t_size& features) const
{
    static const std::string vectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    static const std::string vectorVectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    static const size_t vectorRealType = ParameterTypeSystem::getParameterID(vectorRealMetaName);
    static const size_t vectorVectorRealType = ParameterTypeSystem::getParameterID(vectorVectorRealMetaName);

    auto timeType = features.find(Naming::Feature_time_type);
    auto stateType = features.find(Naming::Feature_state_type);
    if(timeType != features.end() && stateType != features.end()) {
        if(timeType->second == ParameterTypeSystem::pid_real) {
            if(stateType->second == vectorRealType || stateType->second == vectorVectorRealType) {
                return true;
            }
        }
    }

    return false;
}

const std::string RungeKuttaRegistry::getSystemName() const
{
    return "ode";
}

RungeKutta_double_vecDouble::RungeKutta_double_vecDouble(
                       TemplateOdeSystem<double, ublas::vector<double> > * system,
                       double dt
                       )
{
    //ctor

    _rk = new odeint::runge_kutta4<ublas::vector<double> >();
    _dt = dt;
    _system = new RungeKuttaSystem<double, boost::numeric::ublas::vector<double> >(system);
    _time = 0;
    int length = system->getFeatures()[Naming::Feature_size];
    _state = new ublas::vector<double>(length);
}

RungeKutta_double_vecDouble::~RungeKutta_double_vecDouble()
{
    delete _rk;
    delete _state;
    delete _system;
}

size_t RungeKutta_double_vecDouble::getTimeType() const
{
    return ParameterTypeSystem::getParameterID(Naming::Type_real);
}

size_t RungeKutta_double_vecDouble::getStateType() const
{
    static const std::string vectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    return ParameterTypeSystem::getParameterID(vectorRealMetaName);
}

//double * RungeKutta::getTime()
void * RungeKutta_double_vecDouble::currentTime()
{
    return new double(_time);
}

//struct T_VectorDef* RungeKutta::getState()
void * RungeKutta_double_vecDouble::currentState()
{
    return new struct T_VectorDef({_state->size(), true, false, _state});
}

const double& RungeKutta_double_vecDouble::getTime() const
{
    return _time;
}

const boost::numeric::ublas::vector<double>& RungeKutta_double_vecDouble::getState() const
{
    return *_state;
}

bool RungeKutta_double_vecDouble::nextStep()
{
    if(_time >= _endtime)
        return false;
    _rk->do_step( *_system , *_state , _time , _dt );
    _time += _dt;

    return true;
}

void RungeKutta_double_vecDouble::initialize(vec_t_LuaItem args)
{
    if(args.size() > 1) {
        if(args[0].type == ParameterTypeSystem::pid_real && args[1].type == getStateType()) {
            _time = *((double*)args[0].value);
            struct T_VectorDef* vd = (struct T_VectorDef*)args[1].value;
            delete _state;
            _state = new ublas::vector<double>(*((ublas::vector<double>*)vd->value));
        }
    } else {
        if(args.size() > 0 && args[0].type == getStateType()) {
            struct T_VectorDef* vd = (struct T_VectorDef*)args[0].value;
            delete _state;
            _state = new ublas::vector<double>(*((ublas::vector<double>*)vd->value));
        }
    }
}

void RungeKutta_double_vecDouble::start(vec_t_LuaItem args)
{
    if(args.size() > 0 && args[0].type == ParameterTypeSystem::pid_real) {
        _endtime = _time + *((double*)args[0].value);
    } else {
        _endtime = _time + 1.0;
    }
}

///////////////////////////////////////////////////////////
/*
RungeKutta_double_vecvecDouble::RungeKutta_double_vecvecDouble(
                       TemplateOdeSystem<double, vec_vec_real> * system,
                       double dt
                       )
{
    //ctor
    _rk = new odeint::runge_kutta4<vec_vec_real>();
    _dt = dt;
    _time = 0;
    _system = new RungeKuttaSystem<double, vec_vec_real>(system);
    int length = system->getFeatures()[Naming::Feature_size];
    _state = new vec_vec_real(length);
}

RungeKutta_double_vecvecDouble::~RungeKutta_double_vecvecDouble()
{
    //dtor
}

size_t RungeKutta_double_vecvecDouble::getTimeType() const
{
    return ParameterTypeSystem::getParameterID(Naming::Type_real);
}

size_t RungeKutta_double_vecvecDouble::getStateType() const
{
    const std::string vectorVectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    return ParameterTypeSystem::getParameterID(vectorVectorRealMetaName);
}

//double * RungeKutta::getTime()
void * RungeKutta_double_vecvecDouble::currentTime()
{
    return &_time;
}

//struct T_VectorDef* RungeKutta::getState()
void * RungeKutta_double_vecvecDouble::currentState()
{
    return new struct T_VectorDef({_state->size(), true, false, _state});
}

bool RungeKutta_double_vecvecDouble::nextStep()
{
    if(_time > 5)
        return false;


    _time++;

    return true;
}

void RungeKutta_double_vecvecDouble::initialize(vec_t_LuaItem args)
{

}

void RungeKutta_double_vecvecDouble::start(vec_t_LuaItem args)
{

}
*/
