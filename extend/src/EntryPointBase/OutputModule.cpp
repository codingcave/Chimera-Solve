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
#include "EntryPointBase/OutputModule.hpp"

chimera::simulation::OutputModule::OutputModule()
{
    //ctor
}

chimera::simulation::OutputModule::~OutputModule()
{
    //dtor
}

void* chimera::simulation::OutputModule::getInstance(vec_t_LuaItem& parameters) const
{
    return (void*)getOutputInstance(parameters);
}

