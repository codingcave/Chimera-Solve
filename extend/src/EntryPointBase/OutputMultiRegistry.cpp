#include <iostream>
#include <unordered_map>
#include <vector>

#include "ParameterType.hpp"
#include "def.hpp"
#include "Naming.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "EntryPointBase/OutputMultiRegistry.hpp"

OutputMultiRegistry::OutputMultiRegistry()
{
    //ctor
}

OutputMultiRegistry::~OutputMultiRegistry()
{
    //dtor
}

void* OutputMultiRegistry::getInstance(vec_t_LuaItem& parameters) const
{
    return (void*)getOutputInstance(parameters);
}
