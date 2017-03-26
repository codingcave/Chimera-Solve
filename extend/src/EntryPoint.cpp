#include <iostream>
#include <unordered_map>
#include <vector>

#include "LoggingSystem.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "util.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"

EntryPoint::EntryPoint()
{
    _loadedRegistries = new map_t_Registry();
}

EntryPoint::~EntryPoint()
{
    delete _loadedRegistries;
}

bool EntryPoint::load(const std::string& name, Registry * const reg)
{
    bool result = false;
    if(reg->getEntrypoint() == getName() && EntryPointSystem::_instance._luaparser)
    {
        result = onLoad(reg);
        if(result)
        {
            _loadedRegistries->insert (std::make_pair(name, reg));
            EntryPointSystem::_instance._luaparser->notifyLoad(name, this, reg);
        }
    }
    return result;
}

void EntryPoint::unload(const std::string& name)
{
    if(EntryPointSystem::_instance._luaparser) {
        map_t_Registry::iterator available = _loadedRegistries->find (name);
        if ( available != _loadedRegistries->end() ) // if loaded
        {
            EntryPointSystem::_instance._luaparser->notifyUnload(name, this, available->second);
            _loadedRegistries->erase(name);
        }
    }
}

void EntryPoint::unloadAll()
{
    if(EntryPointSystem::_instance._luaparser)
    {
        for (auto it = _loadedRegistries->begin(); it != _loadedRegistries->end(); it++)
        {
            EntryPointSystem::_instance._luaparser->notifyUnload(it->first, this, it->second);
        }
        _loadedRegistries->clear();
    }
}

const Registry* EntryPoint::getRegistry(const std::string& name) const
{
    map_t_Registry::const_iterator available = _loadedRegistries->find (name);

    if ( available != _loadedRegistries->end() ) // if not loaded
    {
        return available->second;
    }
    else
    {
        return nullptr;
    }
}

bool EntryPoint::onLoad(Registry const * const reg)
{
    // virtual - do nothing
    return true;
}

void EntryPoint::init(lua_State* L)
{
    // virtual, do nothing
}

int EntryPoint::size() const
{
    return _loadedRegistries->size();
}

std::ostream& operator<< (std::ostream &out, const EntryPoint &d)
{
    out << d.getName();
    return out;
}

EntryPoint::iterator::iterator (map_t_EntryPoint::iterator it):
    _it(it)
{

}

EntryPoint::iterator::~iterator()
{

}

EntryPoint* EntryPoint::iterator::operator*()
{
    return _it->second;
}

EntryPoint::iterator EntryPoint::iterator::operator++()
{
    EntryPoint::iterator it(++_it);
    return it;
}

EntryPoint::iterator EntryPoint::iterator::operator++(int)
{
    EntryPoint::iterator it(_it++);
    return it;
}

bool EntryPoint::iterator::operator==(const EntryPoint::iterator& other)
{
    return _it == other._it;
}

bool EntryPoint::iterator::operator!=(const EntryPoint::iterator& other)
{
    return _it != other._it;
}
