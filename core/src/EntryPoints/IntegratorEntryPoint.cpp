#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include "lua.hpp"

//#include "interfaces/EventHandler/IParameterTypeEventHandler.hpp"
#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/IntegratorRegistry.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/IntegratorEntryPoint.hpp"

IntegratorEntryPoint::IntegratorEntryPoint()
{
    //ctor
}

IntegratorEntryPoint::~IntegratorEntryPoint()
{
    //dtor
}

const std::string IntegratorEntryPoint::getName() const
{
    return Naming::EntryPoint_integrator;
}

const std::string IntegratorEntryPoint::getVersion() const
{
    return "1.0.0";
}

bool IntegratorEntryPoint::onLoad(Registry const * const reg)
{
    const IntegratorRegistry* ireg = dynamic_cast<const IntegratorRegistry*>(reg);
    return ireg != nullptr;
}

void IntegratorEntryPoint::init(lua_State* L)
{

}
