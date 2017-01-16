#include <iostream>
#include <unordered_map>
#include "lua.hpp"

#include "ParameterType.hpp"

ParameterType::ParameterType():
    _id(-1),
    _name(""),
    _initialize(nullptr),
    _pushValue(nullptr),
    _deleteValue(nullptr),
    _base(0),
    _tag(0)
{
    //ctor
}

ParameterType::ParameterType(const std::string& name, const int& id, const struct T_ParameterDef& pdef):
    _id(id),
    _name(name),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _base(0),
    _tag(0)
{
    //ctor
}

ParameterType::ParameterType(const std::string& name, const int& id, const struct T_ParameterDef& pdef, size_t base, size_t tag):
    _id(id),
    _name(name),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _base(base),
    _tag(tag)
{
    //ctor
}


ParameterType::~ParameterType()
{

}

int ParameterType::getID() const
{
    return _id;
}

const std::string ParameterType::getName() const
{
    return _name;
}

size_t ParameterType::getTag() const
{
    return _tag;
}

size_t ParameterType::getBase() const
{
    return _base;
}

void ParameterType::initialize(lua_State * const L) const
{
    if(_initialize)
        (*_initialize)(L, _id);
}

void ParameterType::initialize(lua_State * const L, const int& id) const
{
    if(_initialize)
        (*_initialize)(L, id);
}

void ParameterType::pushValue(lua_State * const L, void * const value) const
{
    if(_pushValue)
        (*_pushValue)(L, value, _id);
}

void ParameterType::deleteValue(lua_State* const L, void * const value) const
{
    if(_deleteValue)
        (*_deleteValue)(L, value, _id);
}
