#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
//#include <chrono>
#include <random>

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
#include "EntryPointBase/AbstractRandomGenerator.hpp"
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/RandomModule.hpp"
#include "UniformRandom.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new UniformRandomModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (UniformRandomModule*)module;
    }
}

UniformRandomModule::UniformRandomModule()
{
    //ctor
}

UniformRandomModule::~UniformRandomModule()
{
    //dtor
}

const std::string UniformRandomModule::getGUID() const
{
    return "Uniform Distribution";
}

void UniformRandomModule::destroyInstance(void* instance) const
{
    delete ((UniformDistribution*)instance);
}

const std::string UniformRandomModule::getVersion() const
{
    return "1.0.0";
}

const size_t UniformRandomModule::getType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

chimera::simulation::AbstractRandom* UniformRandomModule::getRandomInstance(chimera::simulation::AbstractRandomGenerator* generator, chimera::vec_t_LuaItem& parameters) const
{
    if(parameters.size() != 2 ||
        chimera::systemtypes::PID_NUMBER != parameters[0].getType() ||
        chimera::systemtypes::PID_NUMBER != parameters[1].getType())
    {
        return nullptr;
    }

    double a = parameters[0];
    double b = parameters[1];

    if(a < b)
    {
        return new UniformDistribution(generator, getChimeraSystem()->getTypeSystem(), a, b);
    }
    return nullptr;
}

UniformDistribution::UniformDistribution(chimera::simulation::AbstractRandomGenerator* generator, chimera::ParameterTypeSystem* ps, double a, double b)
{
    _uniRealDist = new std::uniform_real_distribution<double>(a, b);
    _generator = generator;
}

UniformDistribution::~UniformDistribution()
{
    delete _uniRealDist;
}

const size_t UniformDistribution::getType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

void* UniformDistribution::getNext()
{
    return new double((*_uniRealDist)(*_generator));
}
