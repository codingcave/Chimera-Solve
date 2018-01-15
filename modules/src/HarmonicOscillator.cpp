#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>

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
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "HarmonicOscillator.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new HarmonicOscillatorModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (HarmonicOscillatorModule*)module;
    }
}

HarmonicOscillatorModule::HarmonicOscillatorModule()
{
    //setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});
    //registerMethod("test", rk_test);
}

void* HarmonicOscillatorModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    double a = 0.0;
    double b = 0.0;

    if(parameters.size() > 0 && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
    {
        a = parameters[0];
    }
    if(parameters.size() > 1 && chimera::systemtypes::PID_NUMBER == parameters[1].getType())
    {
        b = parameters[1];
    }

    return new HarmonicOscillator(getChimeraSystem()->getTypeSystem(), a, b);
}

const std::string HarmonicOscillatorModule::getGUID() const
{
    return "Harmonic Oscillator";
}

void HarmonicOscillatorModule::destroyInstance(void* instance) const
{
    delete ((HarmonicOscillator*)instance);
}

const std::string HarmonicOscillatorModule::getVersion() const
{
    return "1.0.0";
}

HarmonicOscillator::HarmonicOscillator(chimera::ParameterTypeSystem* ps, double a, double b):
    _a(a),
    _b(b),
    _ps(ps)
{

}

void HarmonicOscillator::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t)
{
    dxdt(0) = -x(1) * _a;
    dxdt(1) = x(0) * _b;
}

std::unordered_map<std::string, size_t> HarmonicOscillator::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    features[chimera::simulation::Naming::Feature_time_type] = chimera::systemtypes::PID_NUMBER;
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    features[chimera::simulation::Naming::Feature_state_type] = _ps->getParameterID(vectorRealMetaName);
    features[chimera::simulation::Naming::Feature_size] = 2;
    return features;
}

const std::string HarmonicOscillator::getSystemName() const
{
    return "ode";
}

HarmonicOscillator::~HarmonicOscillator()
{

}
