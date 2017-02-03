#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include "lua.hpp"

//#include "interfaces/EventHandler/IParameterTypeEventHandler.hpp"
#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/OutputRegistry.hpp"

//#include "EntryPointBase/AbstractIntegrator.hpp"
//#include "EntryPointBase/IntegratorRegistry.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/OutputEntryPoint.hpp"

OutputEntryPoint::OutputEntryPoint()
{
    //ctor
}

OutputEntryPoint::~OutputEntryPoint()
{
    //dtor
}


const std::string OutputEntryPoint::getName() const
{
    return Naming::EntryPoint_output;
}

const std::string OutputEntryPoint::getVersion() const
{
    return "1.0.0";
}

bool OutputEntryPoint::onLoad(Registry const * const reg)
{
    const OutputRegistry* ireg = dynamic_cast<const OutputRegistry*>(reg);
    return ireg != nullptr;
}

void OutputEntryPoint::init(lua_State* L)
{

}

