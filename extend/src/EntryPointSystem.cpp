#include <iostream>
#include <unordered_map>
#include <vector>

#include "LoggingSystem.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"

EntryPointSystem EntryPointSystem::_instance;

EntryPointSystem::EntryPointSystem()
{
    _hooks = new map_t_EntryPoint();
    _luaparser = nullptr;
}

EntryPointSystem::~EntryPointSystem()
{
    disconnect();
    for (auto mIt = _hooks->begin(); mIt != _hooks->end(); mIt++) {
        delete mIt->second;
    }
    delete _hooks;
}

void EntryPointSystem::addEntryPoint(EntryPoint* ep)
{
    map_t_EntryPoint::const_iterator available = _instance._hooks->find (ep->getName());

    if ( available == _instance._hooks->end() )
    {
        std::cout << "EP: " << ep->getName() << std::endl;
        _instance._hooks->insert(std::make_pair(ep->getName(),ep));
        if(_instance._luaparser)
        {
            _instance._luaparser->notifyLoad(ep);
        }
    }
    else
    {
        std::string s = "entry point `";
        s += ep->getName();
        s += "` already set";
        LoggingSystem::Error(s);
    }
}

EntryPoint::iterator EntryPointSystem::begin()
{
    EntryPoint::iterator it(_instance._hooks->begin());
    return it;
}

EntryPoint::iterator EntryPointSystem::end()
{
    EntryPoint::iterator it(_instance._hooks->end());
    return it;
}

EntryPoint* const EntryPointSystem::get(const std::string& name)
{
    map_t_EntryPoint::const_iterator available = _instance._hooks->find (name);

    if ( available != _instance._hooks->end() )
    {
        return available->second;
    }
    return nullptr;
}

void EntryPointSystem::connect(IConnectEventHandler* const c)
{
    disconnect();
    _instance._luaparser = c;
}

void EntryPointSystem::disconnect()
{
    if(_instance._luaparser){
        for (auto mIt = _instance._hooks->begin(); mIt != _instance._hooks->end(); mIt++) {
            mIt->second->truncate();
            mIt->second->unloadAll();
        }
        _instance._luaparser = nullptr;
    }
}
