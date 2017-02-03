#include <iostream>
#include <unordered_map>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

//#include "interfaces/EventHandler/IParameterTypeEventHandler.hpp"
#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "FitzHughNagumo.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    Registry* LoadModule()
    {
        return new FitzHughNagumoRegistry();
    }
}

FitzHughNagumo::FitzHughNagumo(double epsilon, double a):
    _epsilon(epsilon),
    _a(a)
{
    std::cout << "FHN construct " << _epsilon << " " << _a << std::endl;
}

void FitzHughNagumo::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t)
{
    //static double _1_3 = 1.0/3.0;
    //dxdt(0) = x(0) - _1_3 * x(0) * x(0) * x(0) - x(1);
    //dxdt(1) = (x(0) + _a) / _epsilon;
    dxdt(0) = x(1);
    dxdt(1) = -x(0) - _epsilon*x(1);
}

std::unordered_map<std::string, size_t> FitzHughNagumo::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    features[Naming::Feature_time_type] = ParameterTypeSystem::getParameterID(Naming::Type_real);
    static const std::string vectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    features[Naming::Feature_state_type] = ParameterTypeSystem::getParameterID(vectorRealMetaName);
    features[Naming::Feature_size] = 2;
    return features;
}

const std::string FitzHughNagumo::getSystemName() const
{
    return "ode";
}

FitzHughNagumo::~FitzHughNagumo()
{

}

int rk_test(lua_State* L)
{
    FitzHughNagumo* inc = *((FitzHughNagumo**)lua_touserdata(L, lua_upvalueindex(1)));
    std::cout << "RK epsilon = " << inc->_epsilon << " -- a = " << inc->_a << std::endl;

    return 0;
}

FitzHughNagumoRegistry::FitzHughNagumoRegistry()
{
    setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});

    registerMethod("test", rk_test);
}

void* FitzHughNagumoRegistry::getInstance(vec_t_LuaItem& parameters) const
{
    double a = 0.0;
    double epsilon = 0.0;

    std::cout << "FitzHughNagumoRegistry" << std::endl;
    if(parameters.size() > 0 && ParameterTypeSystem::isParameterID(Naming::Type_real, parameters[0].type))
    {
        epsilon = *((double*)parameters[0].value);
        std::cout << "FHNR: eps = " << epsilon << std::endl;
    }
    if(parameters.size() > 1 && ParameterTypeSystem::isParameterID(Naming::Type_real, parameters[0].type))
    {
        a = *((double*)parameters[1].value);
        std::cout << "FHNR: a = " << a << std::endl;
    }
    //for(int i = 0; i < parameters.size(); i++)
    //    std::cout << ParameterTypeSystem::getParameterName(parameters[i].type) << std::endl;

    return new FitzHughNagumo(epsilon, a);
}

const std::string FitzHughNagumoRegistry::getEntrypoint() const
{
    return Naming::EntryPoint_dynamics;
}

void FitzHughNagumoRegistry::destroyInstance(void* instance) const
{
    delete ((FitzHughNagumo*)instance);
}

const std::string FitzHughNagumoRegistry::getVersion() const
{
    return "1.0.0";
}
