#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
//#include "lua.hpp"

#include "Naming.hpp"
#include "StateSynchrony.hpp"
#include "types/lua_basetypes.hpp"
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

chimera::ChimeraSystem::ChimeraSystem()
{
    _loggingSys = new LoggingSystem();
    _typeSys = new ParameterTypeSystem();
    _typeSys->_chimeraSystem = this;
    _epSys = new EntryPointSystem();
    _epSys->_chSys = this;

    _epSys->addListener(this);
    _typeSys->addListener(this);

    init();
}

/*
chimera::ChimeraSystem::ChimeraSystem(ParameterTypeSystem* typeSys, EntryPointSystem* epSys, LoggingSystem* logSys)
{
    _loggingSys = logSys;
    _typeSys = typeSys;
    _epSys = epSys;

    init();
}
*/

chimera::ChimeraSystem::~ChimeraSystem()
{
    delete _loggingSys;
    if(_typeSys != nullptr) {
        delete _typeSys;
    }
    if(_epSys != nullptr) {
        delete _epSys;
    }
}

chimera::LoggingSystem* chimera::ChimeraSystem::getLoggingSystem()
{
    return _loggingSys;
}

chimera::ParameterTypeSystem* chimera::ChimeraSystem::getTypeSystem()
{
    return _typeSys;
}

chimera::EntryPointSystem* chimera::ChimeraSystem::getEntryPointSystem()
{
    return _epSys;
}

chimera::Module* chimera::ChimeraSystem::includeModule(chimera::EntryPoint* entryPoint, const std::string& name)
{
    Module* modPtr = this->openModule(entryPoint, name);
    if(modPtr) {
        if(entryPoint->addModule(name, modPtr))
        {
            return modPtr;
        }
        else
        {
            closeModule(modPtr);
        }
    }
    return nullptr;
}

size_t chimera::ChimeraSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag)
{
    return getTypeSystem()->registerParameter(name, this, pdef, base, tag);
}

size_t chimera::ChimeraSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef)
{
    return getTypeSystem()->registerParameter(name, this, pdef);
}

size_t chimera::ChimeraSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags)
{
    return getTypeSystem()->registerParameter(name, this, pdef, base, tag, flags);
}

size_t chimera::ChimeraSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& flags)
{
    return getTypeSystem()->registerParameter(name, this, pdef, flags);
}

/*
void chimera::ChimeraSystem::pushLuaValue(lua_State* const L, chimera::ParameterValue& param) const
{
    _typeSys->pushValue(L, param.getType(), param.getValue());
}

void chimera::ChimeraSystem::pushLuaValue(lua_State* const L, size_t type, void* value) const
{
    _typeSys->pushValue(L, type, value);
}

void chimera::ChimeraSystem::pushLuaValue(lua_State* const L, const std::string& type, void* value) const
{
    _typeSys->pushValue(L, _typeSys->getParameterID(type), value);
}
*/

void chimera::ChimeraSystem::deleteLuaValue(lua_State* const L, ParameterValue& value) const
{
    _typeSys->deleteValue(L, value);
}

void chimera::ChimeraSystem::deleteLuaValue(lua_State* const L, size_t type, void* value) const
{
    _typeSys->deleteValue(L, type, value);
}

chimera::Module* chimera::ChimeraSystem::openModule(chimera::EntryPoint const * const entrypoint, const std::string& name)
{
    return nullptr;
}

void chimera::ChimeraSystem::closeModule(Module* module)
{
    // virtual - do nothing
}

void chimera::ChimeraSystem::notifyItemAdded(StateSynchrony* sender, void const * const item, void const * const data)
{
    if(sender == _typeSys)
    {
        // ParameterType
        pushType((ParameterType*)item);
    }
    else if(sender == _epSys)
    {
        // EntryPoint
    }
    else
    {
        // Module
        chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        std::string* name = (std::string*)data;
        chimera::Module* module = (chimera::Module*)item;
        if(module->isLoaded())
        {
            pushModule(entrypoint, *name, module);
        }
    }
}

void chimera::ChimeraSystem::notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data)
{
    if(sender == _typeSys)
    {
        // ParameterType
        popType(*((ParameterType*)item));
    }
    else if(sender == _epSys)
    {
        // EntryPoint
    }
    else
    {
        // Module
        chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        //std::string* name = (std::string*)data;
        chimera::Module* module = (chimera::Module*)item;
        popModule(entrypoint, module);
    }
}

void chimera::ChimeraSystem::notifyItemLoaded(StateSynchrony* sender, void const * const item, void const * const data)
{
    if(sender == _epSys)
    {
        // EntryPoint
        //std::string* name = (std::string*)data;
        EntryPoint* entrypoint = (EntryPoint*)item;
        entrypoint->addListener(this);
        std::string name = getEntryPointSystem()->findEntryPoint((const EntryPoint*)item);
        pushEntryPoint(name, entrypoint);
        for(auto it = entrypoint->_loadedModules->begin(); it != entrypoint->_loadedModules->end(); it++)
        {
            it->second->addListener(this);
            if(it->second->isLoaded())
            {
                pushModule(entrypoint, it->first, it->second);
            }
        }
    }
    else
    {
        // Module
        //chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        //const chimera::Module* module = (const chimera::Module*)item;
        chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        std::string* name = (std::string*)data;
        chimera::Module* module = (chimera::Module*)item;
        pushModule(entrypoint, *name, module);
    }
}

void chimera::ChimeraSystem::notifyItemUnloaded(StateSynchrony* sender, void const * const item, void const * const data)
{
    std::string* name = (std::string*)data;
    if(sender == _epSys)
    {
        // EntryPoint
        EntryPoint* entrypoint = (EntryPoint*)item;
        entrypoint->removeListener(this);
        popEntryPoint(*name, entrypoint);
    }
    else
    {
        // Module
        chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        const chimera::Module* module = (const chimera::Module*)item;
        popModule(entrypoint, module);
    }
}

const std::string chimera::ChimeraSystem::getGUID() const
{
    return "Chimerasystem";
}

void chimera::ChimeraSystem::setLogLevel(const unsigned int& loglevel)
{
    _loggingSys->setLogLevel(loglevel);
}
