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
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/IntegratorRegistry.hpp"

IntegratorRegistry::IntegratorRegistry()
{
    //ctor
}

IntegratorRegistry::~IntegratorRegistry()
{
    //dtor

}

void* IntegratorRegistry::getInstance(vec_t_LuaItem& parameters) const
{
    return (void*)getIntegratorInstance(parameters);
}
