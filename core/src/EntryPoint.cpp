#include <iostream>
#include <map>
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
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"

chimera::EntryPoint::EntryPoint()
{
    _loadedModules = new map_t_Module();
}

chimera::EntryPoint::~EntryPoint()
{
    unloadEntryPoint();
    auto name = _chimeraSystem->getEntryPointSystem()->findEntryPoint(this);
    _chimeraSystem->getEntryPointSystem()->removeEntryPoint(name);
    delete _loadedModules;
}

bool chimera::EntryPoint::addModule(const std::string& name, Module * const module)
{
    bool result = false;
    result = checkModule(module);
    if(result)
    {
        module->_chimeraSystem = _chimeraSystem;
        _loadedModules->insert (std::make_pair(name, module));
        itemAdded(module, &name);
        module->addListener(this);
    }
    return result;
}

void chimera::EntryPoint::removeModule(const std::string& name)
{
    map_t_Module::iterator available = _loadedModules->find (name);
    if ( available != _loadedModules->end() ) // if loaded
    {
        available->second->unloadModule();
        itemRemoved(available->second, &name);
        //available->second->removeListener(this);
        _loadedModules->erase(name);
    }
}

const chimera::Module* chimera::EntryPoint::getModule(const std::string& name) const
{
    map_t_Module::const_iterator available = _loadedModules->find (name);

    if ( available != _loadedModules->end() ) // if not loaded
    {
        return available->second;
    }
    else
    {
        return nullptr;
    }
}

const std::string chimera::EntryPoint::findModule(Module const * const module) const
{
    for(auto it = _loadedModules->begin(); it != _loadedModules->end(); it++)
    {
        if(it->second == module)
        {
            return it->first;
        }
    }
    return "";
}

bool chimera::EntryPoint::checkModule(chimera::Module const * const mod) const
{
    // virtual - do nothing
    return true;
}

void chimera::EntryPoint::init(lua_State* L)
{
    // virtual, do nothing
    return;
}

int chimera::EntryPoint::size() const
{
    return _loadedModules->size();
}

chimera::Module::iterator chimera::EntryPoint::begin()
{
    chimera::Module::iterator it(_loadedModules->begin());
    return it;
}

chimera::Module::iterator chimera::EntryPoint::end()
{
    chimera::Module::iterator it(_loadedModules->end());
    return it;
}

std::ostream& operator<< (std::ostream &out, const chimera::EntryPoint &d)
{
    out << d.getGUID() << "-" << d.getVersion();
    return out;
}

chimera::ChimeraSystem* chimera::EntryPoint::getChimeraSystem() const
{
    return _chimeraSystem;
}

void chimera::EntryPoint::load()
{
    // virtual, do nothing
}

void chimera::EntryPoint::unload()
{
    // virtual, do nothing
}

void const * const chimera::EntryPoint::getModuleLoadParams() const
{
    // virtual, do nothing
    return nullptr;
}

void chimera::EntryPoint::loadEntryPoint(ChimeraSystem * chimerasystem)
{
    _chimeraSystem = chimerasystem;
    if(!isLoaded())
    {
        stateLoaded();
        load();
    }
}

void chimera::EntryPoint::unloadEntryPoint()
{
    if(isLoaded())
    {
        for(auto it = _loadedModules->begin(); it != _loadedModules->end(); it++)
        {
            it->second->unloadModule();
        }

        unload();
        stateUnloaded();
    }
}

/*
void chimera::EntryPoint::notifyUnload(StateSynchrony* sender, void const * const data)
{
    for(auto it = _loadedModules->begin(); it != _loadedModules->end(); it++)
    {
        if(it->second == sender)
        {
            //it->second->removeListener(this);
            //itemRemoved(it->second, &it->first);
            //_loadedModules->erase(it->first);
        }
    }
}*/

void chimera::EntryPoint::notifyDelete(StateSynchrony* sender)
{
    for(auto it = _loadedModules->begin(); it != _loadedModules->end(); it++)
    {
        if(it->second == sender)
        {
            std::string name(it->first);
            sender->removeListener(this);
            //itemRemoved(sender, &name);
            _loadedModules->erase(name);
            break;
        }
    }
}

chimera::EntryPoint::iterator::iterator (map_t_EntryPoint::iterator it):
    _it(it)
{

}

chimera::EntryPoint::iterator::~iterator()
{

}

chimera::EntryPoint* chimera::EntryPoint::iterator::operator*()
{
    return _it->second;
}

chimera::EntryPoint::iterator chimera::EntryPoint::iterator::operator++()
{
    EntryPoint::iterator it(++_it);
    return it;
}

chimera::EntryPoint::iterator chimera::EntryPoint::iterator::operator++(int)
{
    EntryPoint::iterator it(_it++);
    return it;
}

bool chimera::EntryPoint::iterator::operator==(const chimera::EntryPoint::iterator& other)
{
    return _it == other._it;
}

bool chimera::EntryPoint::iterator::operator!=(const chimera::EntryPoint::iterator& other)
{
    return _it != other._it;
}
