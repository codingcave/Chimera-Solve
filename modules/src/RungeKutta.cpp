#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
//#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "interfaces/IEventListener.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/TemplateIntegrator.hpp"
#include "EntryPointBase/IntegratorRegistry.hpp"
#include "RungeKutta.hpp"

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

const std::string RungeKuttaRegistry::getEntrypoint() const
{
    return Naming::EntryPoint_integrator;
}

const std::string RungeKuttaRegistry::getVersion() const
{
    return "1.0.0";
}


void RungeKuttaRegistry::destroyInstance(void * const instance) const
{
    delete (RungeKutta*)instance;
    //delete (TemplateIntegrator
    //        <double, double>
    //        *)instance;
}

AbstractIntegrator* RungeKuttaRegistry::getIntegratorInstance(vec_t_LuaItem& parameters) const
{
    std::cout << "RK Construct S" << std::endl;
    RungeKutta* rk = new RungeKutta(nullptr);
    //TemplateIntegrator
    //<double, double>
    //* rk = new TemplateIntegrator
    //<double, double>
    //();
    //std::cout << "RK Construct " << rk << std::endl;
    return rk;
    //return nullptr;
}

bool RungeKuttaRegistry::checkFeatures(const map_t_size& features) const
{
    return true;
}

const std::string RungeKuttaRegistry::getSystemName() const
{
    return "ode";
}

RungeKutta::RungeKutta(
                       TemplateOdeSystem<double, double> * system
                       )
{
    //ctor
    _rk = new odeint::runge_kutta4< double >();
    _system = system;
    _time = 0;
    _state = 0;
}

RungeKutta::~RungeKutta()
{
    //dtor
}

size_t RungeKutta::getTimeType() const
{
    return ParameterTypeSystem::getParameterID(Naming::Type_real);
}

size_t RungeKutta::getStateType() const
{
    return ParameterTypeSystem::getParameterID(Naming::Type_real);
}

double * RungeKutta::getTime()
{
    return &_time;
}

double * RungeKutta::getState()
{
    return &_state;
}

bool RungeKutta::nextStep()
{
    if(_time > 5)
        return false;
    _time++;
    _state+=3;

    return true;
}

void RungeKutta::initialize(vec_t_LuaItem args)
{

}
