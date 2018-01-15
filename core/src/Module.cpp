#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"

chimera::Module::Module()
{
    _instanceMethods = new std::unordered_map<std::string, chimera::fn_instancefnwrapper>();
}

chimera::Module::~Module()
{
    delete _instanceMethods;
    unloadModule();
}

void chimera::Module::registerMethod(const std::string& name, chimera::fn_instancefnwrapper method)
{
    (*_instanceMethods)[name] = method;
}

std::unordered_map<std::string, chimera::fn_instancefnwrapper> const * const chimera::Module::methods() const
{
    return _instanceMethods;
}

void chimera::Module::load(EntryPoint const * const entryPoint, void const * const params)
{

    return;
}

void chimera::Module::unload()
{

    return;
}

void chimera::Module::loadModule(EntryPoint const * const entryPoint, const std::string& name, void const * const params)
{
    if(!isLoaded())
    {
        _cmSys = entryPoint->getChimeraSystem();
        load(entryPoint, params);
        stateLoaded(&name);
    }
}

void chimera::Module::unloadModule()
{
    if(isLoaded())
    {
        unload();
        stateUnloaded();
    }
}

chimera::ChimeraSystem* chimera::Module::getChimeraSystem() const
{
    return _cmSys;
}

chimera::Module::iterator::iterator (map_t_Module::iterator it):
    _it(it)
{

}

chimera::Module::iterator::~iterator()
{

}

chimera::Module* chimera::Module::iterator::operator*()
{
    return _it->second;
}

chimera::Module::iterator chimera::Module::iterator::operator++()
{
    chimera::Module::iterator it(++_it);
    return it;
}

chimera::Module::iterator chimera::Module::iterator::operator++(int)
{
    chimera::Module::iterator it(_it++);
    return it;
}

bool chimera::Module::iterator::operator==(const chimera::Module::iterator& other)
{
    return _it == other._it;
}

bool chimera::Module::iterator::operator!=(const chimera::Module::iterator& other)
{
    return _it != other._it;
}
