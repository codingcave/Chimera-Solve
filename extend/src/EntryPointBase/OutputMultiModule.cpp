#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "StateSynchrony.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
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
