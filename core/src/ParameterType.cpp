#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "def.hpp"
#include "ParameterType.hpp"


chimera::ParameterType::ParameterType():
    _id(-1),
    _name(""),
    _origin(nullptr),
    _initialize(nullptr),
    _pushValue(nullptr),
    _deleteValue(nullptr),
    _dispose(nullptr),
    _base(0),
    _tag(0)
{
    //ctor
}

chimera::ParameterType::ParameterType(const std::string& name, const int& id, void const * const origin, const struct chimera::T_ParameterDef& pdef):
    _id(id),
    _name(name),
    _origin(origin),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _dispose(pdef.dispose),
    _base(0),
    _tag(0)
{
    //ctor
}

chimera::ParameterType::ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag):
    _id(id),
    _name(name),
    _origin(origin),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _dispose(pdef.dispose),
    _base(base),
    _tag(tag)
{
    //ctor
}

chimera::ParameterType::~ParameterType()
{

}

int chimera::ParameterType::getID() const
{
    return _id;
}

const std::string chimera::ParameterType::getName() const
{
    return _name;
}

void const * const chimera::ParameterType::getOrigin() const
{
    return _origin;
}

size_t chimera::ParameterType::getTag() const
{
    return _tag;
}

size_t chimera::ParameterType::getBase() const
{
    return _base;
}

void chimera::ParameterType::initialize(lua_State * const L) const
{
    initialize(L, _id);
}

void chimera::ParameterType::initialize(lua_State * const L, const int& id) const
{
    if(_initialize)
    {
        lua_pushcfunction(L, _initialize);
        lua_insert(L, -2);
        lua_pushinteger(L, id);
        lua_call(L, 2, 1);
    }
}

void chimera::ParameterType::pushValue(lua_State * const L, void * const value) const
{
    if(_pushValue)
    {
        lua_pushcfunction(L, _pushValue);
        lua_pushinteger(L, _id);
        lua_pushlightuserdata(L, value);
        lua_call(L, 2, 1);
    }
}

void chimera::ParameterType::deleteValue(lua_State* const L, void * const value) const
{
    if(_deleteValue)
    {
        lua_pushcfunction(L, _deleteValue);
        lua_pushinteger(L, _id);
        lua_pushlightuserdata(L, value);
        lua_call(L, 2, 0);
    }
}

bool chimera::ParameterType::castFrom(lua_State* const L, size_t type, void * const value) const
{
    lua_CFunction fn_cast = nullptr;
    auto cast = _castFuntions.find(type);
    if(cast != _castFuntions.end())
    {
        fn_cast = cast->second;
    } else {
        cast = _castFuntions.find(0);
        if(cast != _castFuntions.end())
        {
            fn_cast = cast->second;
        }
    }

    if(fn_cast)
    {
        lua_pushcfunction(L, fn_cast);
        lua_pushinteger(L, type);
        lua_pushlightuserdata(L, value);
        lua_call(L, 2, 1);
        if(ParameterType::getType(L, -1) == _id)
        {
            return true;
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    return false;
}

void chimera::ParameterType::addCast(size_t from, lua_CFunction fn_cast)
{
    _castFuntions[from] = fn_cast;
}

void chimera::ParameterType::dispose(lua_State* const L) const
{
    if(_dispose)
    {
        lua_pushcfunction(L, _dispose);
        lua_pushlightuserdata(L, (void*)this);
        lua_pushlightuserdata(L, (void*)_tag);
        lua_call(L, 2, 0);
    }
}

size_t chimera::ParameterType::getType(lua_State* const L, const int& index)
{
    switch(lua_type(L, index))
    {
    case LUA_TNONE:
    case LUA_TNIL:
        {
            return chimera::systemtypes::PID_NIL;
        }
    case LUA_TBOOLEAN:
        {
            return chimera::systemtypes::PID_BOOLEAN;
        }
    case LUA_TNUMBER:
        {
            return chimera::systemtypes::PID_NUMBER;
        }
    case LUA_TSTRING:
        {
            return chimera::systemtypes::PID_STRING;
        }
    case LUA_TTABLE:
        {
            return chimera::systemtypes::PID_TABLE;
        }
    case LUA_TFUNCTION:
        {
            return chimera::systemtypes::PID_FUNCTION;
        }
    case LUA_TUSERDATA:
        {
            if(luaL_getmetafield(L, index, "__type"))
            {
                int isnum, type;
                type = lua_tointegerx(L, -1, &isnum);
                lua_pop(L, 1);
                if(isnum)
                {
                    return type;
                }
            }
        }
    }

    return chimera::systemtypes::PID_NIL;
}
