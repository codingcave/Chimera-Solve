#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include "lua.hpp"

//#include "interfaces/EventHandler/IParameterTypeEventHandler.hpp"
//#include "RuntimeNames.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/EventManager.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"
#include "EntryPointBase/OutputModule.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/OutputEntryPoint.hpp"

chimera::runtime::OutputEntryPoint::OutputEntryPoint()
{
    //ctor
}

chimera::runtime::OutputEntryPoint::~OutputEntryPoint()
{
    //dtor
}


const std::string chimera::runtime::OutputEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_output;
}

const std::string chimera::runtime::OutputEntryPoint::getVersion() const
{
    return "1.0.0";
}

bool chimera::runtime::OutputEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::OutputModule* ireg = dynamic_cast<const chimera::simulation::OutputModule*>(module);
    if(ireg)
        return true;
    const chimera::simulation::OutputMultiModule* mireg = dynamic_cast<const chimera::simulation::OutputMultiModule*>(module);
    return mireg != nullptr;
}

void chimera::runtime::OutputEntryPoint::init(lua_State* L)
{

}

