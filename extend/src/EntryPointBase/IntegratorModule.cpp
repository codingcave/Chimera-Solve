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
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"

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


