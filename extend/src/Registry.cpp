#include <iostream>
#include <unordered_map>
#include <vector>

#include "util.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"

Registry::Registry()
{
    _instanceMethods = new std::unordered_map<std::string, lua_CFunction>();
}

Registry::~Registry()
{
    delete _instanceMethods;
}

void Registry::registerMethod(const std::string& name, lua_CFunction method)
{
    (*_instanceMethods)[name] = method;
}

std::unordered_map<std::string, lua_CFunction> const * const Registry::methods() const
{
    return _instanceMethods;
}

void Registry::load()
{

}

void Registry::unload()
{

}
