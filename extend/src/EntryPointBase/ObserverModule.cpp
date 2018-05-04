#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <list>

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
#include "interfaces/IEventListener.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "EntryPointBase/ObserverModule.hpp"

chimera::simulation::ObserverModule::ObserverModule()
{
    //ctor
}

chimera::simulation::ObserverModule::~ObserverModule()
{
    //dtor
}

void* chimera::simulation::ObserverModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getEventInstance(parameters);
}
