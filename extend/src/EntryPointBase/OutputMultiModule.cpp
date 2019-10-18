#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ExtensionNaming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "StateSynchrony.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"

chimera::simulation::OutputMultiModule::OutputMultiModule()
{
    //ctor
}

chimera::simulation::OutputMultiModule::~OutputMultiModule()
{
    //dtor
}

void* chimera::simulation::OutputMultiModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getOutputInstance(parameters);
}

size_t chimera::simulation::OutputMultiModule::getFlag(const std::string& flag) const
{
    if (flag == chimera::simulation::Naming::Flag_MultiObserver)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
