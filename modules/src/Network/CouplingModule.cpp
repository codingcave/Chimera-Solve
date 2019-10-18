#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "Network/AbstractCoupling.hpp"
#include "Network/CouplingModule.hpp"

CouplingModule::CouplingModule()
{
    //ctor
}

CouplingModule::~CouplingModule()
{
    //dtor
}

void* CouplingModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getCoupling(parameters);
}

void CouplingModule::destroyInstance(void * const instance) const
{
    delete (AbstractCoupling*)instance;
}
