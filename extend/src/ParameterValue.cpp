#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"

ParameterValue::ParameterValue(size_t type, void* value)
{
    if(type == 0 || value == nullptr)
    {
        _type = type;
        _value = nullptr;
        _disposed = false;
        _references = nullptr;
        _luaref = nullptr;
    }
    else
    {
        auto refValue = ParameterTypeSystem::_instance._references->find(value);

        if(refValue == ParameterTypeSystem::_instance._references->end())
        {
            //std::cout << "New PV: " << type << "|" << ParameterTypeSystem::getParameterName(type) << " :: " << value << std::endl;
            _type = type;
            _disposed = false;
            _value = value;
            _references = new int(0);
            _luaref = new bool(false);

            ParameterTypeSystem::_instance._references->insert(
                std::make_pair(value,*this)
            );
        }
        else
        {
            _type = refValue->second._type;
            _disposed = false;
            _value = refValue->second._value;
            _references = refValue->second._references;
            ++(*_references);
            _luaref = refValue->second._luaref;
            //std::cout << "NCopy PV: " << refValue->second._type << "|" << ParameterTypeSystem::getParameterName(refValue->second._type) << " :: " << refValue->second._value << " => " << *refValue->second._references << std::endl;
        }
    }
}

ParameterValue::ParameterValue(const ParameterValue& p)
{
    if(p._type == 0 || p._value == nullptr)
    {
        _type = p._type;
        _disposed = false;
        _value = nullptr;
        _references = nullptr;
        _luaref = nullptr;
    }
    else
    {
        _type = p._type;
        _disposed = false;
        _value = p._value;
        _references = p._references;
        ++(*_references);
        _luaref = p._luaref;
        //std::cout << "Copy PV: " << p._type << "|" << ParameterTypeSystem::getParameterName(p._type) << " :: " << p._value << " => " << *p._references << std::endl;
    }
}

ParameterValue::~ParameterValue()
{
    dispose();
}

size_t ParameterValue::getType() const
{
    return _type;
}

void* const ParameterValue::operator-> ()
{
    return _value;
}

void* ParameterValue::getValue() const
{
    return _value;
}

void ParameterValue::dispose()
{
    if(!_disposed)
    {
        _disposed = true;

        if(_value != nullptr)
        {
            --(*_references);
            //std::cout << "DEL PV: " << _type << "|" << ParameterTypeSystem::getParameterName(_type) << " :: " << _value << " => " << *_references << std::endl;
            if(*_references == 0 && !*_luaref)
            {
                //std::cout << "DISPOSE PV: " << std::endl;
                auto it = ParameterTypeSystem::_instance._references->find(_value);
                if(it != ParameterTypeSystem::_instance._references->end())
                {
                    ParameterTypeSystem::deleteValue(*this);
                    it->second.dispose();
                    ParameterTypeSystem::_instance._references->erase(_value);
                }

                delete _references;
                delete _luaref;
            }
        }
    }
}
