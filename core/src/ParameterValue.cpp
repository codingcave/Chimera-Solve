#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "Naming.hpp"
//#include "interfaces/IConnectEventHandler.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "types/lua_basetypes.hpp"
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

chimera::ParameterReferenceInteger::ParameterReferenceInteger(const chimera::ParameterValue& value, size_t idx)
    :_inner(value)
{
    _idx = idx;
}

chimera::ParameterReferenceInteger::~ParameterReferenceInteger()
{
    _inner.dispose();
}

chimera::ParameterReferenceInteger& chimera::ParameterReferenceInteger::operator=(const chimera::ParameterValue& other)
{
    ChimeraSystem* c1 = _inner.getChimeraSystem();
    ChimeraSystem* c2 = other.getChimeraSystem();
    if(c1 != nullptr && (c2 == nullptr || c1 == c2))
    {
        if(_inner.getType() == chimera::systemtypes::PID_TABLE)
        {
            map_t_LuaItem* table = (map_t_LuaItem*)_inner.getValue();
            table->erase(std::to_string(_idx));
            table->insert(std::make_pair(std::to_string(_idx), other));
            other.bind(c1);
        }
        else
        {
            other.bind(c1);
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {_inner.getType(), _inner.getValue()});
            if(luaL_getmetafield(L, -1, "__newindex"))
            {
                lua_pop(L, 1);
                c1->getTypeSystem()->pushValue(L, {other.getType(), other.getValue()});
                lua_seti(L, -2, _idx);
            }

            lua_pop(L, 1);
        }
    } else {
        // THROW
    }
    return *this;
}


chimera::ParameterValue chimera::ParameterReferenceInteger::getValue() const
{
    if(_inner.getType() == chimera::systemtypes::PID_TABLE)
    {
        map_t_LuaItem* table = (map_t_LuaItem*)_inner.getValue();
        auto it = table->find(std::to_string(_idx));
        if(it != table->end())
        {
            return it->second;
        }
    }
    else
    {
        ChimeraSystem* c1 = _inner.getChimeraSystem();
        if(c1)
        {
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {_inner.getType(), _inner.getValue()});
            if(luaL_getmetafield(L, -1, "__index"))
            {
                lua_pop(L, 1);
                lua_geti(L, -1, _idx);
                ParameterValue p = c1->getTypeSystem()->getValue(L, -1);
                lua_pop(L, 2);
                return p;
            }

            lua_pop(L, 1);
        }
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterReferenceString::ParameterReferenceString(const chimera::ParameterValue& value, std::string idx)
    :_inner(value)
{
    _idx = idx;
}

chimera::ParameterReferenceString::~ParameterReferenceString()
{
    _inner.dispose();
}

chimera::ParameterReferenceString& chimera::ParameterReferenceString::operator=(const chimera::ParameterValue& other)
{
    ChimeraSystem* c1 = _inner.getChimeraSystem();
    ChimeraSystem* c2 = other.getChimeraSystem();
    if(c1 != nullptr && (c2 == nullptr || c1 == c2))
    {
        if(_inner.getType() == chimera::systemtypes::PID_TABLE)
        {
            map_t_LuaItem* table = (map_t_LuaItem*)_inner.getValue();
            table->erase(_idx);
            table->insert(std::make_pair(_idx, other));
            if(c1)
            {
                other.bind(c1);
            }
        }
        else
        {
            other.bind(c1);
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {_inner.getType(), _inner.getValue()});

            if(luaL_getmetafield(L, -1, "__newindex"))
            {
                lua_pop(L, 1);
                c1->getTypeSystem()->pushValue(L, {other.getType(), other.getValue()});
                lua_setfield(L, -2, _idx.c_str());
            }

            lua_pop(L, 1);
        }
    }
    return *this;
}

chimera::ParameterValue chimera::ParameterReferenceString::getValue() const
{
    if(_inner.getType() == chimera::systemtypes::PID_TABLE)
    {
        map_t_LuaItem* table = (map_t_LuaItem*)_inner.getValue();
        auto it = table->find(_idx);
        if(it != table->end())
        {
            return it->second;
        }
    }
    else
    {
        ChimeraSystem* c1 = _inner.getChimeraSystem();
        if(c1)
        {
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {_inner.getType(), _inner.getValue()});
            if(luaL_getmetafield(L, -1, "__index"))
            {
                lua_pop(L, 1);
                lua_pushlstring(L, _idx.c_str(), _idx.size());
                lua_gettable(L, -2);
                ParameterValue p = c1->getTypeSystem()->getValue(L, -1);
                lua_pop(L, 2);
                return p;
            }

            lua_pop(L, 1);
        }
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterValue::ParameterValue()
    :chimera::ParameterValue::ParameterValue(0, nullptr)
{

}

chimera::ParameterValue::ParameterValue(chimera::ChimeraSystem* cmSys, size_t type, void* value)
{
    if(type == 0 || value == nullptr)
    {
        _data = nullptr;
        _disposed = false;
    }
    else
    {
        auto refValue = cmSys->getTypeSystem()->_references->find(value);

        if(refValue == cmSys->getTypeSystem()->_references->end())
        {
            _data = new ParameterValueData({
                cmSys,
                type,
                value,
                0,
                nullptr
            });
            _disposed = false;

            cmSys->getTypeSystem()->_references->insert(
                std::make_pair(value,*this)
            );
        }
        else
        {
            _data = refValue->second._data;
            ++(_data->references);
            _disposed = false;
        }
    }
}

chimera::ParameterValue::ParameterValue(size_t type, void* value)
{
    if(type == 0 || value == nullptr)
    {
        _data = nullptr;
        _disposed = false;
    }
    else
    {
        _data = new ParameterValueData({
            nullptr,
            type,
            value,
            1,
            nullptr
        });
        _disposed = false;
    }
}

chimera::ParameterValue::ParameterValue(const chimera::ParameterValue& p)
{
    if(p._data)
    {
        _data = p._data;
        ++(_data->references);
    }
    else
    {
        _data = nullptr;
    }
    _disposed = false;
}

chimera::ParameterValue::ParameterValue(const double& value)
    :ParameterValue(chimera::systemtypes::PID_NUMBER, new double(value))
{

}

chimera::ParameterValue::ParameterValue(const int& value)
    :ParameterValue(chimera::systemtypes::PID_NUMBER, new double((double)value))
{

}

chimera::ParameterValue::ParameterValue(const bool& value)
    :ParameterValue(chimera::systemtypes::PID_BOOLEAN, new bool(value))
{

}

chimera::ParameterValue::ParameterValue(const std::string& value)
    :ParameterValue(chimera::systemtypes::PID_STRING, new std::string(value))
{

}

chimera::ParameterValue::ParameterValue(const char value[])
    :ParameterValue(chimera::systemtypes::PID_STRING, new std::string(value))
{

}

/*
chimera::ParameterValue::ParameterValue(const std::unordered_map<std::string, chimera::ParameterValue>& value)
    :ParameterValue(ParameterTypeSystem::pid_table, new std::unordered_map<std::string, chimera::ParameterValue>(value))
{
    ChimeraSystem* c1 = nullptr;
    ChimeraSystem* c2 = nullptr;
    for(auto it = value.begin(); it != value.end(); it++)
    {
        c2 = (*it).second.getChimeraSystem();
        if(c1 == nullptr)
        {
            c1 = c2;
        }
        else
        {
            if(c2 != nullptr && c1 != c2)
            {
                // THROW
            }
        }
    }
    bind(c1);
}
*/

chimera::ParameterValue::ParameterValue(const chimera::ParameterReferenceString& value)
    :ParameterValue(value.getValue())
{

}

chimera::ParameterValue::ParameterValue(const chimera::ParameterReferenceInteger& value)
    :ParameterValue(value.getValue())
{

}

chimera::ParameterValue::~ParameterValue()
{
    dispose();
}

size_t chimera::ParameterValue::getType() const
{
    if(_data)
    {
        return _data->type;
    }
    else
    {
        return 0;
    }
}

void* chimera::ParameterValue::getValue() const
{
    if(_data)
    {
        return _data->value;
    }
    else
    {
        return nullptr;
    }
}

chimera::ChimeraSystem* chimera::ParameterValue::getChimeraSystem(struct ParameterValueData* data)
{
    if(data)
    {
        return data->cmSys;
    }
    return nullptr;
}

chimera::ChimeraSystem* chimera::ParameterValue::getChimeraSystem() const
{
    if(_data)
    {
        return _data->cmSys;
    }
    return nullptr;
}

void chimera::ParameterValue::bind(ChimeraSystem* chSys) const
{
    if(_data && _data->cmSys == nullptr && chSys != nullptr)
    {
        _data->cmSys = chSys;
        auto refValue = chSys->getTypeSystem()->_references->find(getValue());

        if(refValue == chSys->getTypeSystem()->_references->end())
        {
            chSys->getTypeSystem()->_references->insert(
                std::make_pair(_data->value, *this)
            );
            --(_data->references);
        }
        else
        {
            delete _data;
            // ERROR: Cannot bind
        }
    }
}

void* const chimera::ParameterValue::operator-> ()
{
    return getValue();
}

void chimera::ParameterValue::dispose()
{
    if(!_disposed)
    {
        _disposed = true;

        if(getValue())
        {
            --(_data->references);
            if(_data->references == 0 && !_data->luaref)
            {
                if(_data->cmSys)
                {
                    _data->cmSys->getTypeSystem()->deleteValue(_data->cmSys->getLuaState(), *this);
                    auto it = _data->cmSys->getTypeSystem()->_references->find(_data->value);
                    if(it != _data->cmSys->getTypeSystem()->_references->end())
                    {
                        it->second.dispose();
                        _data->cmSys->getTypeSystem()->_references->erase(_data->value);
                    }
                }
                else
                {
                    switch(getType()) {
                    /*
                        case ParameterTypeSystem::pid_real:
                        {
                            luat_real_delete(nullptr, getValue(), getType());
                            break;
                        }
                        case ParameterTypeSystem::pid_boolean:
                        {
                            luat_boolean_delete(nullptr, getValue(), getType());
                            break;
                        }
                        case ParameterTypeSystem::pid_string:
                        {
                            luat_string_delete(nullptr, getValue(), getType());
                            break;
                        }
                        case ParameterTypeSystem::pid_table:
                        {
                            luat_table_delete(nullptr, getValue(), getType());
                            break;
                        }
                        case ParameterTypeSystem::pid_func:
                        {
                            // NOT IMPLEMENTED!
                            break;
                        }
                        case ParameterTypeSystem::pid_library:
                        {
                            // DO NOTHING!
                            break;
                        }
                        case ParameterTypeSystem::pid_luafunc:
                        {
                            // NOT IMPLEMENTED!
                            break;
                        }
                        */
                    }
                }

                delete _data;
            }
        }
    }
}

chimera::ParameterValue chimera::ParameterValue::arith1(const chimera::ParameterValue& p1, int op)
{
    ChimeraSystem* c = p1.getChimeraSystem();
    if(c)
    {
        lua_State* L = c->getLuaState();
        c->getTypeSystem()->pushValue(L, {p1.getType(), p1.getValue()});
        lua_arith(L, op);
        ParameterValue v(c->getTypeSystem()->getValue(L, -1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterValue chimera::ParameterValue::arith2(const chimera::ParameterValue& p1, const chimera::ParameterValue& p2, int op)
{
    ChimeraSystem* c1 = p1.getChimeraSystem();
    ChimeraSystem* c2 = p1.getChimeraSystem();
    if(c1 == nullptr || c2 == nullptr || c1 == c2)
    {
        c1 = c1 ? c1 : c2;
        if(c1)
        {
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {p1.getType(), p1.getValue()});
            c1->getTypeSystem()->pushValue(L, {p2.getType(), p2.getValue()});
            lua_arith(L, op);
            ParameterValue v(c1->getTypeSystem()->getValue(L, -1));
            lua_pop(L, 1);
            return v;
        }
    }
    return ParameterValue(0, nullptr);
}


chimera::ParameterValue chimera::ParameterValue::compare(const chimera::ParameterValue& p1, const chimera::ParameterValue& p2, int op)
{
    ChimeraSystem* c1 = p1.getChimeraSystem();
    ChimeraSystem* c2 = p1.getChimeraSystem();
    if(c1 == nullptr || c2 == nullptr || c1 == c2)
    {
        c1 = c1 ? c1 : c2;
        if(c1)
        {
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {p1.getType(), p1.getValue()});
            c1->getTypeSystem()->pushValue(L, {p2.getType(), p2.getValue()});
            bool result = lua_compare(L, -2, -1, op);
            lua_pop(L, 2);
            return result;
        }
    }
    return false;
}

chimera::ParameterValue chimera::ParameterValue::operator+(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPADD);
}

chimera::ParameterValue chimera::ParameterValue::operator-(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPSUB);
}

chimera::ParameterValue chimera::ParameterValue::operator*(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPMUL);
}

chimera::ParameterValue chimera::ParameterValue::operator/(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPDIV);
}

chimera::ParameterValue chimera::ParameterValue::operator%(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPMOD);
}

chimera::ParameterValue chimera::ParameterValue::operator^(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPPOW);
}

chimera::ParameterValue chimera::ParameterValue::operator-()
{
    return arith1(*this, LUA_OPUNM);
}

chimera::ParameterValue chimera::ParameterValue::operator&(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPBAND);
}

chimera::ParameterValue chimera::ParameterValue::operator|(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPBOR);
}

chimera::ParameterValue chimera::ParameterValue::XOR(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPBXOR);
}

chimera::ParameterValue chimera::ParameterValue::operator~()
{
    return arith1(*this, LUA_OPBNOT);
}

chimera::ParameterValue chimera::ParameterValue::operator<<(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPSHL);
}

chimera::ParameterValue chimera::ParameterValue::operator<<(const int& i)
{
    ChimeraSystem* c1 = getChimeraSystem();
    if(c1)
    {
        lua_State* L = c1->getLuaState();
        c1->getTypeSystem()->pushValue(L, {getType(), getValue()});
        lua_pushnumber(L, i);
        lua_arith(L, LUA_OPSHL);
        ParameterValue v(c1->getTypeSystem()->getValue(L, -1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterValue chimera::ParameterValue::operator>>(const chimera::ParameterValue& p)
{
    return arith2(*this, p, LUA_OPSHR);
}

chimera::ParameterValue chimera::ParameterValue::operator>>(const int& i)
{
    ChimeraSystem* c1 = getChimeraSystem();
    if(c1)
    {
        lua_State* L = c1->getLuaState();
        c1->getTypeSystem()->pushValue(L, {getType(), getValue()});
        lua_pushnumber(L, i);
        lua_arith(L, LUA_OPSHR);
        ParameterValue v(c1->getTypeSystem()->getValue(L, -1));
        lua_pop(L, 1);
        return v;
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterValue chimera::ParameterValue::concat(const chimera::ParameterValue& p)
{
    ChimeraSystem* c1 = getChimeraSystem();
    ChimeraSystem* c2 = p.getChimeraSystem();
    if(c1 == nullptr || c2 == nullptr || c1 == c2)
    {
        c1 = c1 ? c1 : c2;
        if(c1)
        {
            lua_State* L = c1->getLuaState();
            c1->getTypeSystem()->pushValue(L, {getType(), getValue()});
            c1->getTypeSystem()->pushValue(L, {p.getType(), p.getValue()});
            lua_concat(L, 2);
            ParameterValue v(c1->getTypeSystem()->getValue(L, -1));
            lua_pop(L, 1);
            return v;
        }
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterValue chimera::ParameterValue::length()
{
    // REVIEW: return int?
    ChimeraSystem* c = getChimeraSystem();
    if(c)
    {
        lua_State* L = c->getLuaState();
        c->getTypeSystem()->pushValue(L, {getType(), getValue()});
        lua_len(L, -1);
        ParameterValue v(c->getTypeSystem()->getValue(L, -1));
        lua_pop(L, 2);
        return v;
    }
    return ParameterValue(0, nullptr);
}

bool chimera::ParameterValue::operator==(const chimera::ParameterValue& p)
{
    return compare(*this, p, LUA_OPEQ);
}

bool chimera::ParameterValue::operator!=(const chimera::ParameterValue& p)
{
    return !compare(*this, p, LUA_OPEQ);
}

bool chimera::ParameterValue::operator<(const chimera::ParameterValue& p)
{
    return compare(*this, p, LUA_OPLT);
}

bool chimera::ParameterValue::operator>(const chimera::ParameterValue& p)
{
    return compare(p, *this, LUA_OPLT);
}

bool chimera::ParameterValue::operator<=(const chimera::ParameterValue& p)
{
    return compare(*this, p, LUA_OPLE);
}

bool chimera::ParameterValue::operator>=(const chimera::ParameterValue& p)
{
    return compare(p, *this, LUA_OPLE);
}

chimera::ParameterReferenceInteger chimera::ParameterValue::operator[](std::size_t idx)
{
    return chimera::ParameterReferenceInteger(*this, idx);
}

const chimera::ParameterReferenceInteger chimera::ParameterValue::operator[](std::size_t idx) const
{
    return chimera::ParameterReferenceInteger(*this, idx);
}

chimera::ParameterReferenceString chimera::ParameterValue::operator[](std::string idx)
{
    return chimera::ParameterReferenceString(*this, idx);
}

const chimera::ParameterReferenceString chimera::ParameterValue::operator[](std::string idx) const
{
    return chimera::ParameterReferenceString(*this, idx);
}

chimera::vec_t_LuaItem chimera::ParameterValue::operator()(chimera::vec_t_LuaItem& params)
{
    ChimeraSystem* c = getChimeraSystem();
    if(c)
    {
        lua_State* L = c->getLuaState();

        int pos1 = lua_gettop(L);

        c->getTypeSystem()->pushValue(L, {getType(), getValue()});
        for(auto it : params)
        {
            c->getTypeSystem()->pushValue(L, {it.getType(), it.getValue()});
        }
        lua_call(L, params.size(), LUA_MULTRET);
        int pos2 = lua_gettop(L);
        vec_t_LuaItem result;
        for(int i = pos1 + 1; i <= pos2; ++i)
        {
            result.push_back(c->getTypeSystem()->getValue(L, i));
        }
        if(pos2 > pos1)
        {
            lua_pop(L, pos2 - pos1);
        }
        return result;
    }
    return vec_t_LuaItem();
}

chimera::ParameterValue chimera::ParameterValue::call_single(std::vector<chimera::ParameterValue>& params)
{
    ChimeraSystem* c = getChimeraSystem();
    if(c)
    {
        lua_State* L = c->getLuaState();
        c->getTypeSystem()->pushValue(L, {getType(), getValue()});
        for(auto it : params)
        {
            c->getTypeSystem()->pushValue(L, {it.getType(), it.getValue()});
        }
        lua_call(L, params.size(), 1);
        ParameterValue result = c->getTypeSystem()->getValue(L, -1);
        lua_pop(L, 1);
        return result;
    }
    return ParameterValue(0, nullptr);
}

chimera::ParameterValue chimera::ParameterValue::operator()(chimera::ParameterValue p1)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    return call_single(params);
}

chimera::ParameterValue chimera::ParameterValue::operator()(chimera::ParameterValue p1, chimera::ParameterValue p2)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    params.push_back(p2);
    ParameterValue pr = ParameterValue(call_single(params));
    return pr;
}

chimera::ParameterValue chimera::ParameterValue::operator()(chimera::ParameterValue p1, chimera::ParameterValue p2, chimera::ParameterValue p3)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    params.push_back(p2);
    params.push_back(p3);
    return call_single(params);
}

chimera::ParameterValue chimera::ParameterValue::operator()(chimera::ParameterValue p1, chimera::ParameterValue p2, chimera::ParameterValue p3, chimera::ParameterValue p4)
{
    vec_t_LuaItem params;
    params.push_back(p1);
    params.push_back(p2);
    params.push_back(p3);
    params.push_back(p4);
    return call_single(params);
}

chimera::ParameterValue::operator int() const
{
    int result = 0;
    if(getType() == chimera::systemtypes::PID_NUMBER)
    {
        result = (int)(*((double*)getValue()));
    }
    return result;
}

chimera::ParameterValue::operator double() const
{
    double result = 0;
    if(getType() == chimera::systemtypes::PID_NUMBER)
    {
        result = *((double*)getValue());
    }
    return result;
}

chimera::ParameterValue::operator bool() const
{
    bool result = false;
    if(getType() == chimera::systemtypes::PID_BOOLEAN)
    {
        result = *((bool*)getValue());
    }
    return result;
}

chimera::ParameterValue::operator std::string() const
{
    std::string result;
    if(getType() == chimera::systemtypes::PID_STRING)
    {
        result = *((std::string*)getValue());
    }
    return result;
}

/*
chimera::ParameterValue::operator chimera::map_t_LuaItem() const
{
    map_t_LuaItem result;
    if(_type == ParameterTypeSystem::pid_table)
    {
        result = *((map_t_LuaItem*)_value);
    }
    return result;
}

chimera::ParameterValue::operator std::vector<chimera::ParameterValue>() const
{
    std::vector<ParameterValue> result;
    result.push_back(*this);
    return result;
}
*/

chimera::ParameterValue::operator void*() const
{
    return getValue();
}

std::ostream& chimera::operator<<(std::ostream& os, const chimera::ParameterValue& obj)
{
    chimera::ChimeraSystem* c = obj.getChimeraSystem();
    if(c)
    {
        lua_State* L = c->getLuaState();
        c->getTypeSystem()->pushValue(L, {obj.getType(), obj.getValue()});
        os << luaL_tolstring(L, -1, NULL);
        lua_pop(L, 2);
    }
    return os;
}

std::ostream& chimera::operator<<(std::ostream& os, const chimera::ParameterReferenceInteger& obj)
{
    chimera::ParameterValue p = obj;
    os << obj;
    p.dispose();
    return os;
}

std::ostream& chimera::operator<<(std::ostream& os, const chimera::ParameterReferenceString& obj)
{
    chimera::ParameterValue p = obj;
    os << obj;
    p.dispose();
    return os;
}

