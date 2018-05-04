#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ItemContainer.hpp"
#include "StateSynchrony.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/InitializerModule.hpp"

chimera::simulation::InitializerModule::InitializerModule()
{
    //ctor
}

chimera::simulation::InitializerModule::~InitializerModule()
{
    //dtor
}

void* chimera::simulation::InitializerModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getInitializerInstance(parameters);
}
