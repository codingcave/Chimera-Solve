#include <iostream>
#include <map>
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
#include "NormalRandom.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new NormalRandomModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (NormalRandomModule*)module;
    }
}

NormalRandomModule::NormalRandomModule()
{
    //ctor
}

NormalRandomModule::~NormalRandomModule()
{
    //dtor
}

const std::string NormalRandomModule::getGUID() const
{
    return "Normal Distribution";
}

void NormalRandomModule::destroyInstance(void* instance) const
{
    delete ((NormalRandomModule*)instance);
}

const std::string NormalRandomModule::getVersion() const
{
    return "1.0.0";
}

const size_t NormalRandomModule::getType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

chimera::simulation::AbstractRandom* NormalRandomModule::getRandomInstance(chimera::simulation::AbstractRandomGenerator* generator, chimera::vec_t_LuaItem& parameters) const
{
    double mean = 0.0;
    double sigma;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSigma;
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "mean" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                mean = p.second;
            }
            if(p.first == "sigma" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inSigma = true;
                sigma = p.second;
            }
        }
        if(inSigma && sigma > 0)
        {
            return new NormalDistribution(generator, getChimeraSystem()->getTypeSystem(), mean, sigma);
        }
    }
    else
    {
        if((parameters.size() == 1 || parameters.size() == 2) && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
        {
            if(parameters.size() == 2)
            {
                if(chimera::systemtypes::PID_NUMBER == parameters[1].getType())
                {
                    mean = parameters[0];
                    sigma = parameters[1];
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                sigma = parameters[0];
            }
            if(sigma > 0)
            {
                return new NormalDistribution(generator, getChimeraSystem()->getTypeSystem(), mean, sigma);
            }
        }
    }
    return nullptr;
}

NormalDistribution::NormalDistribution(chimera::simulation::AbstractRandomGenerator* generator, chimera::ParameterTypeSystem* ps, double mean, double sigma)
{
    _normDist = new std::normal_distribution<double>(mean, sigma);
    _generator = generator;
}

NormalDistribution::~NormalDistribution()
{
    delete _normDist;
}

const size_t NormalDistribution::getType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

void* NormalDistribution::getNext()
{
    return new double((*_normDist)(*_generator));
}
