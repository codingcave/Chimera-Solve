#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/OutputRegistry.hpp"

OutputRegistry::OutputRegistry()
{
    //ctor
}

OutputRegistry::~OutputRegistry()
{
    //dtor
}

void* OutputRegistry::getInstance(vec_t_LuaItem& parameters) const
{
    return (void*)getOutputInstance(parameters);
}

