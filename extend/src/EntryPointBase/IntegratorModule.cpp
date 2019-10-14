#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include "lua.hpp"

#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ItemContainer.hpp"
#include "StateSynchrony.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"

chimera::simulation::IntegratorModule::IntegratorModule()
{
    //ctor
}

chimera::simulation::IntegratorModule::~IntegratorModule()
{
    //dtor

}

void* chimera::simulation::IntegratorModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getIntegratorInstance(parameters);
}


