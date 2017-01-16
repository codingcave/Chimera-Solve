#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

//#include "interfaces/EventHandler/IParameterTypeEventHandler.hpp"
#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/SystemDynamicEntryPoint.hpp"

int lua_sysdyn_use(lua_State* L)
{
    std::cout << "lua_sysdyn_use " << lua_gettop(L) << std::endl;
    return 0;
}

SystemDynamicEntryPoint::SystemDynamicEntryPoint()
{
    _active = nullptr;
    //setValue("use", {ParameterTypeSystem::getParameterID("function"), (void*)lua_sysdyn_use});
}

SystemDynamicEntryPoint::~SystemDynamicEntryPoint()
{
    //dtor
}

const std::string SystemDynamicEntryPoint::getName() const
{
    return Naming::EntryPoint_dynamics;
}

const std::string SystemDynamicEntryPoint::getVersion() const
{
    return "1.0.0";
}


bool SystemDynamicEntryPoint::onLoad(Registry const * const reg)
{
    _active = reg;
    return true;
}

void SystemDynamicEntryPoint::init(lua_State* L)
{
    setValue("use", {ParameterTypeSystem::getParameterID(Naming::Type_function), (void*)lua_sysdyn_use});
}
