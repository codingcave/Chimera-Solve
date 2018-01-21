#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

chimera::EntryPointSystem::EntryPointSystem()
{
    _hooks = new map_t_EntryPoint();
    _chSys = nullptr;
    stateLoaded();
}

chimera::EntryPointSystem::~EntryPointSystem()
{
    for(auto it = _hooks->begin(); it != _hooks->end(); it++)
    {
        it->second->removeListener(this);
        itemRemoved(it->second, &(it->first));
    }
    delete _hooks;
}

void chimera::EntryPointSystem::addEntryPoint(const std::string& name, EntryPoint* ep)
{
    map_t_EntryPoint::const_iterator available = _hooks->find (name);

    if (available == _hooks->end())
    {
        _hooks->insert(std::make_pair(name, ep));
        itemAdded(ep, &name);
        ep->addListener(this);
        ep->loadEntryPoint(_chSys);
    }
    else
    {
        //std::string s = "entry point `";
        //s += ep->getName();
        //s += "` already set";
        //LoggingSystem::Error(s);
#warning LOG (kekstoaster#1#): entry point `type` already set
    }
}

void chimera::EntryPointSystem::removeEntryPoint(const std::string& name)
{
    map_t_EntryPoint::const_iterator available = _hooks->find (name);

    if (available != _hooks->end())
    {
        _hooks->erase(name);
        available->second->unloadEntryPoint();
        itemRemoved(available->second, &name);
        available->second->removeListener(this);
    }
}

const std::string chimera::EntryPointSystem::findEntryPoint(EntryPoint const * const entrypoint) const
{
    for(auto it = _hooks->begin(); it != _hooks->end(); it++)
    {
        if(it->second == entrypoint)
        {
            return it->first;
        }
    }
    return "";
}

chimera::EntryPoint::iterator chimera::EntryPointSystem::begin()
{
    EntryPoint::iterator it(_hooks->begin());
    return it;
}

chimera::EntryPoint::iterator chimera::EntryPointSystem::end()
{
    EntryPoint::iterator it(_hooks->end());
    return it;
}

chimera::EntryPoint* const chimera::EntryPointSystem::getEntryPoint(const std::string& name)
{
    map_t_EntryPoint::const_iterator available = _hooks->find (name);

    if (available != _hooks->end())
    {
        return available->second;
    }
    return nullptr;
}

/*
void chimera::EntryPointSystem::notifyUnload(StateSynchrony* sender, void const * const data)
{
    for(auto it = _hooks->begin(); it != _hooks->end(); it++)
    {
        if(it->second == sender)
        {
            itemRemoved(it->second, &it->first);
            break;
        }
    }
}*/

void chimera::EntryPointSystem::notifyDelete(StateSynchrony* sender)
{
    for(auto it = _hooks->begin(); it != _hooks->end(); it++)
    {
        if(it->second == sender)
        {
            _hooks->erase(it);
            itemRemoved(it->second, &it->first);
            break;
        }
    }
}


const std::string chimera::EntryPointSystem::getGUID() const
{
    return "EntryPointSystem";
}

