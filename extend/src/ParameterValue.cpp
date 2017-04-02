#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "interfaces/IConnectEventHandler.hpp"
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

ParameterValue::ParameterValue(const double& value)
    :ParameterValue(ParameterTypeSystem::pid_real, new double(value))
{

}

ParameterValue::ParameterValue(const int& value)
    :ParameterValue(ParameterTypeSystem::pid_real, new double((double)value))
{

}

ParameterValue::ParameterValue(const bool& value)
    :ParameterValue(ParameterTypeSystem::pid_boolean, new bool(value))
{

}

ParameterValue::ParameterValue(const std::string& value)
    :ParameterValue(ParameterTypeSystem::pid_string, new std::string(value))
{

}

ParameterValue::ParameterValue(const char value[])
    :ParameterValue(ParameterTypeSystem::pid_string, new std::string(value))
{

}

ParameterValue::ParameterValue(const std::unordered_map<std::string, ParameterValue>& value)
    :ParameterValue(ParameterTypeSystem::pid_table, new std::unordered_map<std::string, ParameterValue>(value))
{

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

ParameterValue ParameterValue::operator+(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPADD);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator-(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPSUB);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator*(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPMUL);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator/(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPDIV);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator%(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPMOD);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator^(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPPOW);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator-()
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        lua_arith(L, LUA_OPUNM);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator&(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPBAND);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator|(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPBOR);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::XOR(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPBXOR);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator~()
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        lua_arith(L, LUA_OPBNOT);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator<<(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPSHL);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator<<(const int& i)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        lua_pushnumber(L, i);
        lua_arith(L, LUA_OPSHL);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator>>(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_arith(L, LUA_OPSHR);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator>>(const int& i)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        lua_pushnumber(L, i);
        lua_arith(L, LUA_OPSHR);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::concat(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        lua_concat(L, 2);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::length()
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        lua_len(L, -1);
        ParameterValue v(ParameterTypeSystem::getValue(-1));
        lua_pop(L, 2);
        return v;
    }
    return ParameterValue(0, nullptr);
}

bool ParameterValue::operator==(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        bool result = lua_compare(L, -2, -1, LUA_OPEQ);
        lua_pop(L, 2);
        return result;
    }
    return false;
}

bool ParameterValue::operator!=(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        bool result = lua_compare(L, -2, -1, LUA_OPEQ);
        lua_pop(L, 2);
        return !result;
    }
    return false;
}

bool ParameterValue::operator<(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        bool result = lua_compare(L, -2, -1, LUA_OPLT);
        lua_pop(L, 2);
        return result;
    }
    return false;
}

bool ParameterValue::operator>(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        bool result = lua_compare(L, -1, -2, LUA_OPLT);
        lua_pop(L, 2);
        return result;
    }
    return false;
}

bool ParameterValue::operator<=(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        bool result = lua_compare(L, -2, -1, LUA_OPLE);
        lua_pop(L, 2);
        return result;
    }
    return false;
}

bool ParameterValue::operator>=(const ParameterValue& p)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        ParameterTypeSystem::pushValue({p.getType(), p.getValue()});
        bool result = lua_compare(L, -1, -2, LUA_OPLE);
        lua_pop(L, 2);
        return result;
    }
    return false;
}

// __index

// __newindex

vec_t_LuaItem ParameterValue::operator()(vec_t_LuaItem& params)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();

        int pos1 = lua_gettop(L);

        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        for(auto it : params)
        {
            ParameterTypeSystem::pushValue({it.getType(), it.getValue()});
        }
        lua_call(L, params.size(), LUA_MULTRET);
        int pos2 = lua_gettop(L);
        vec_t_LuaItem result;
        for(int i = pos1 + 1; i <= pos2; ++i)
        {
            result.push_back(ParameterTypeSystem::getValue(i));
        }
        if(pos2 > pos1)
        {
            lua_pop(L, pos2 - pos1);
        }
        return result;
    }
    return vec_t_LuaItem();
}

ParameterValue ParameterValue::call_single(std::vector<ParameterValue>& params)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();

        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        for(auto it : params)
        {
            ParameterTypeSystem::pushValue({it.getType(), it.getValue()});
        }
        lua_call(L, params.size(), 1);
        ParameterValue result = ParameterTypeSystem::getValue(-1);
        lua_pop(L, 1);
        return result;
    }
    return ParameterValue(0, nullptr);
}

ParameterValue ParameterValue::operator()(ParameterValue p1)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    return call_single(params);
}

ParameterValue ParameterValue::operator()(ParameterValue p1, ParameterValue p2)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    params.push_back(p2);
    return call_single(params);
}

ParameterValue ParameterValue::operator()(ParameterValue p1, ParameterValue p2, ParameterValue p3)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    params.push_back(p2);
    params.push_back(p3);
    return call_single(params);
}

ParameterValue ParameterValue::operator()(ParameterValue p1, ParameterValue p2, ParameterValue p3, ParameterValue p4)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    params.push_back(p2);
    params.push_back(p3);
    params.push_back(p4);
    return call_single(params);
}

ParameterValue::operator int() const
{
    /*
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        int isnum = 0;
        int result = lua_tonumberx(L, -1, &isnum);
        lua_pop(L, 1);
        if(isnum)
        {
            return result;
        }
    }
    return 0;
    */
    int result = 0;
    if(_type == ParameterTypeSystem::pid_real)
    {
        result = (int)(*((double*)_value));
    }
    return result;
}

ParameterValue::operator double() const
{
    /*
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        int result = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return result;
    }
    return 0;
    */
    double result = 0;
    if(_type == ParameterTypeSystem::pid_real)
    {
        result = *((double*)_value);
    }
    return result;
}

ParameterValue::operator bool() const
{
    /*
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({this->getType(), this->getValue()});
        bool result = lua_toboolean(L, -1);
        lua_pop(L, 1);
        return result;
    }
    return 0;
    */
    bool result = 0;
    if(_type == ParameterTypeSystem::pid_boolean)
    {
        result = *((bool*)_value);
    }
    return result;
}

ParameterValue::operator std::string() const
{
    std::string result;
    if(_type == ParameterTypeSystem::pid_string)
    {
        result = *((std::string*)_value);
    }
    return result;
}

ParameterValue::operator map_t_LuaItem() const
{
    map_t_LuaItem result;
    if(_type == ParameterTypeSystem::pid_table)
    {
        result = *((map_t_LuaItem*)_value);
    }
    return result;
}

ParameterValue::operator void*() const
{
    return _value;
}

std::ostream& operator<<(std::ostream& os, const ParameterValue& obj)
{
    if(ParameterTypeSystem::_instance._luaparser)
    {
        lua_State* L = ParameterTypeSystem::_instance._luaparser->getLuaState();
        ParameterTypeSystem::pushValue({obj.getType(), obj.getValue()});
        os << luaL_tolstring(L, -1, NULL);
        lua_pop(L, 2);
    }
    return os;
}


