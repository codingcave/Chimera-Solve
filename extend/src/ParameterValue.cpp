#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValue.hpp"

ParameterValue::ParameterValue(size_t type, void* value)
{
    auto refValue = ParameterTypeSystem::_instance._references->find(value);
    /*
    if(refValue == ParameterTypeSystem::_instance._references->end())
    {

    }
    size_t _type;
    bool _disposed;
    void * _value;
    int* _references;
    bool* _luaref;
    */
}

ParameterValue::ParameterValue(const ParameterValue& p)
{

}

ParameterValue::~ParameterValue()
{
    //dtor
}


size_t ParameterValue::getType() const
{
    return 0;
}

void* const& ParameterValue::operator* ()
{
    return nullptr;
}

void ParameterValue::dispose()
{

}
