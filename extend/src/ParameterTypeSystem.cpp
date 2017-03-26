#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "Naming.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "types/lua_basetypes.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "types/lua_vector.hpp"
#include "types/lua_matrix.hpp"
#include "types/lua_matrixrow.hpp"
//#include "Registry.hpp"

const int ParameterTypeSystem::pid_nil = 0;
const int ParameterTypeSystem::pid_real = 1;
const int ParameterTypeSystem::pid_boolean = 2;
const int ParameterTypeSystem::pid_string = 3;
const int ParameterTypeSystem::pid_table = 4;
const int ParameterTypeSystem::pid_func = 5;
const int ParameterTypeSystem::pid_library = 6;
const int ParameterTypeSystem::pid_luafunc = 7;
const int ParameterTypeSystem::pid_entrypoint = 8;
const int ParameterTypeSystem::pid_registry = 9;
const int ParameterTypeSystem::pid_instance = 10;
const int ParameterTypeSystem::pid_vector = 11;
const int ParameterTypeSystem::pid_matrix = 12;
const int ParameterTypeSystem::pid_matrixrow = 13;


ParameterTypeSystem ParameterTypeSystem::_instance;

ParameterTypeSystem::ParameterTypeSystem():
    _lastID(5),
    _typeList(new std::vector<const ParameterType*>(6)),
    _luaparser(nullptr),
    _references(new std::unordered_map<void*, ParameterValue>())
{
    (*_typeList)[pid_nil] = new ParameterType(Naming::Type_null, pid_nil, {nullptr, luat_nil_push, nullptr});
    (*_typeList)[pid_real] = new ParameterType(Naming::Type_real, pid_real, {nullptr, luat_real_push, luat_real_delete});
    (*_typeList)[pid_boolean] = new ParameterType(Naming::Type_boolean, pid_boolean, {nullptr, luat_boolean_push, luat_boolean_delete});
    (*_typeList)[pid_string] = new ParameterType(Naming::Type_string, pid_string, {nullptr, luat_string_push, luat_string_delete});
    (*_typeList)[pid_table] = new ParameterType(Naming::Type_table, pid_table, {nullptr, luat_table_push, luat_table_delete});
    (*_typeList)[pid_func] = new ParameterType(Naming::Type_function, pid_func, {nullptr, luat_function_push, nullptr});
    (*_typeList)[pid_library] = new ParameterType(Naming::Type_Library, pid_func, {nullptr, nullptr, nullptr});
    registerParameter(Naming::Type_UserFunction, {luat_luafunction_init, luat_function_push, luat_luafunction_delete});
    registerParameter(Naming::Type_EntryPoint, {nullptr, nullptr, nullptr});
    registerParameter(Naming::Type_Registry, {nullptr, nullptr, nullptr});
    registerParameter(Naming::Type_Instance, {nullptr, nullptr, nullptr});
    registerParameter(Naming::Type_Vector, {luat_vector_init, nullptr, nullptr});
    registerParameter(Naming::Type_Matrix, {luat_matrix_init, nullptr, nullptr});
    registerParameter(Naming::Type_MatrixRow, {luat_matrixrow_init, nullptr, luat_matrixrow_delete});
}

ParameterTypeSystem::~ParameterTypeSystem()
{
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        delete *it;
    }
    delete _typeList;
}

int ParameterTypeSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef)
{
    return registerParameter(name, pdef, 0, 0, true);
}

int ParameterTypeSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, bool notify)
{
    return registerParameter(name, pdef, 0, 0, notify);
}

int ParameterTypeSystem::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, bool notify)
{
    for(auto it = _instance._typeList->begin(); it != _instance._typeList->end(); it++)
    {
        if( (*it)->getName() == name )
        {
            return -1;
        }
    }
    ++_instance._lastID;
    ParameterType* p = new ParameterType(name, _instance._lastID, pdef, base, tag);
    _instance._typeList->push_back(p);
    if(_instance._luaparser != nullptr && notify) {
        _instance._luaparser->notifyLoad(p);
    }
    return _instance._lastID;
}

int ParameterTypeSystem::registerVector(const int& subtype, const struct T_ParameterDef& pdef)
{
    if(subtype >= 0 && (unsigned)subtype < _instance._typeList->size())
    {
        std::string name(Naming::Type_Vector);
        name += "#";
        name += getParameterName(subtype);

        for(auto it = _instance._typeList->begin(); it != _instance._typeList->end(); it++)
        {
            if( (*it)->getName() == name )
            {
                return -1;
            }
        }
        ++_instance._lastID;
        ParameterType* p = new ParameterType(name, _instance._lastID, pdef, (unsigned)getParameterID(Naming::Type_Vector), (unsigned)subtype);
        _instance._typeList->push_back(p);
        if(_instance._luaparser != nullptr) {
            _instance._luaparser->notifyLoad(p);
        }
        return _instance._lastID;
    }
    return -1;
}

int ParameterTypeSystem::registerMatrix(const int& subtype, const struct T_ParameterDef& pdef)
{
    if(subtype >= 0 && (unsigned)subtype < _instance._typeList->size())
    {
        std::string name(Naming::Type_Matrix);
        name += "#";
        name += getParameterName(subtype);

        for(auto it = _instance._typeList->begin(); it != _instance._typeList->end(); it++)
        {
            if( (*it)->getName() == name )
            {
                return -1;
            }
        }
        ++_instance._lastID;
        ParameterType* p = new ParameterType(name, _instance._lastID, pdef, (unsigned)getParameterID(Naming::Type_Matrix), (unsigned)subtype);
        _instance._typeList->push_back(p);
        if(_instance._luaparser != nullptr) {
            _instance._luaparser->notifyLoad(p);
        }

        std::string namer(Naming::Type_MatrixRow);
        namer += "#";
        namer += getParameterName(subtype);
        ++_instance._lastID;
        p = new ParameterType(namer, _instance._lastID, {nullptr, nullptr, luat_matrixrow_delete}, (unsigned)getParameterID(Naming::Type_MatrixRow), (unsigned)subtype);
        _instance._typeList->push_back(p);
        if(_instance._luaparser != nullptr) {
            _instance._luaparser->notifyLoad(p);
        }
        return _instance._lastID - 1;
    }
    return -1;
}

int ParameterTypeSystem::getParameterID(const std::string& name)
{
    for(auto it = _instance._typeList->begin(); it != _instance._typeList->end(); it++)
    {
        if( (*it)->getName() == name )
        {
            return (*it)->getID();
        }
    }
    return -1;
}

int ParameterTypeSystem::getParameterID(const size_t& base, const size_t& tag)
{
    for(auto it = _instance._typeList->begin(); it != _instance._typeList->end(); it++)
    {
        if( (*it)->getBase() == base && (*it)->getTag() == tag )
        {
            return (*it)->getID();
        }
    }
    return -1;
}

const struct T_Parameter ParameterTypeSystem::getValue(const int& index)
{
    if(_instance._luaparser)
    {
        lua_State* L = _instance._luaparser->getLuaState();
        switch(lua_type(L, index))
        {
        case LUA_TNONE:
        case LUA_TNIL:
            {
                return {pid_nil, nullptr};
            }
        case LUA_TNUMBER:
            {
                return {pid_real, new double(lua_tonumber(L, index))};
            }
        case LUA_TBOOLEAN:
            {
                return {pid_boolean, new bool(lua_toboolean(L, index))};
            }
        case LUA_TSTRING:
            {
                return {pid_string, new std::string(lua_tostring(L, index))};
            }
        case LUA_TTABLE:
            {
                map_t_LuaItem* table = new map_t_LuaItem();
                int absIndex = lua_absindex(L, index);
                lua_pushnil(L);
                while (lua_next(L, absIndex) != 0) {
                    switch(lua_type(L, -2))
                    {
                    case LUA_TNUMBER:
                        {
                            int isnum = 0;
                            int intKey = lua_tointegerx(L, -2, &isnum);
                            if(isnum)
                            {
                                std::string key = std::to_string(intKey);
                                table->insert(std::make_pair(key, getValue(-1)));
                                //(*table)[key] = value;
                            }
                            break;
                        }
                    case LUA_TSTRING:
                        {
                            std::string key(lua_tostring(L, -2));
                            table->insert(std::make_pair(key, getValue(-1)));
                            //(*table)[key] = value;
                            break;
                        }
                    }

                    lua_pop(L, 1);
                }

                return {pid_table, table};
            }
        case LUA_TFUNCTION:
            {
                int f_ind = lua_absindex(L, index);
                lua_pushstring(L, Naming::Lua_reg_functions);
                lua_rawget(L, LUA_REGISTRYINDEX);
                void* w = (void*)(new LuaFunctionWrapper());
                lua_pushvalue(L, f_ind);
                lua_rawsetp(L, -2, w);
                lua_pop(L, 1);

                return {pid_luafunc, w};
            }
        case LUA_TUSERDATA:
            {
                int f_ind = lua_absindex(L, index);
                if(luaL_getmetafield(L, f_ind, "__type"))
                {
                    int isnum, type;
                    type = lua_tointegerx(L, -1, &isnum);
                    lua_pop(L, 1);
                    if(isnum)
                    {
                        if(type > pid_library)
                        {
                            void* value = *((void**)lua_touserdata(L, f_ind));
                            lua_pushstring(L, Naming::Lua_reg_references);
                            lua_rawget(L, LUA_REGISTRYINDEX);
                            lua_pushvalue(L, f_ind);
                            lua_rawsetp(L, -2, value);
                            lua_pop(L, 1);
                            return {type, value};
                        }
                        else
                        {
                            return {type, nullptr};
                        }
                    }
                }
            }
        }
    }

    return {pid_nil, nullptr};
}

int ParameterTypeSystem::getParameterType(const int& index)
{
    if(_instance._luaparser)
    {
        lua_State* L = _instance._luaparser->getLuaState();
        switch(lua_type(L, index))
        {
        case LUA_TNONE:
        case LUA_TNIL:
            {
                return pid_nil;
            }
        case LUA_TBOOLEAN:
            {
                return pid_boolean;
            }
        case LUA_TNUMBER:
            {
                return pid_real;
            }
        case LUA_TSTRING:
            {
                return pid_string;
            }
        case LUA_TTABLE:
            {
                return pid_nil;
            }
        case LUA_TFUNCTION:
            {
                return pid_luafunc;
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
    }

    return pid_nil;
}

const std::string ParameterTypeSystem::getParameterName(const int& id)
{
    if(id >= 0 && (unsigned)id < _instance._typeList->size())
    {
        if((*_instance._typeList)[id] != nullptr)
        {
            return (*_instance._typeList)[id]->getName();
        }
    }
    return "";
}

size_t ParameterTypeSystem::getParameterBase(const int& id)
{
    if(id >= 0 && (unsigned)id < _instance._typeList->size())
    {
        if((*_instance._typeList)[id] != nullptr)
        {
            return (*_instance._typeList)[id]->getBase();
        }
    }
    return 0;
}

size_t ParameterTypeSystem::getParameterTag(const int& id)
{
    if(id >= 0 && (unsigned)id < _instance._typeList->size())
    {
        if((*_instance._typeList)[id] != nullptr)
        {
            return (*_instance._typeList)[id]->getTag();
        }
    }
    return 0;
}

void ParameterTypeSystem::deleteSingle(lua_State* const L, const struct T_Parameter& value)
{
    // this function needs "simulation:com:references"
    // to be on top of the stack
    switch(value.type)
    {
    case pid_nil:
    case pid_func:
        // do nothing
        break;
    case pid_real:
    case pid_boolean:
    case pid_string:
        {
            (*_instance._typeList)[value.type]->deleteValue(L, value.value);
            break;
        }
    default:
        {
            if(lua_rawgetp(L, -1, value.value))
            {
                lua_pushnil(L);
                lua_rawsetp(L, -3, value.value);
            }
            else
            {
                (*_instance._typeList)[value.type]->deleteValue(L, value.value);
            }
            lua_pop(L, 1);
            break;
        }
    }
}

void ParameterTypeSystem::deleteValues(vec_t_LuaItem& items)
{
    if(_instance._luaparser) {
        lua_State* L = _instance._luaparser->getLuaState();
        lua_pushstring(L, Naming::Lua_reg_references);
        lua_rawget(L, LUA_REGISTRYINDEX);
        for(auto it = items.begin(); it != items.end(); it++)
        {
            deleteSingle(L, *it);
        }
        lua_pop(L, 1);
        items.clear();
    }
}

void ParameterTypeSystem::deleteValue(const struct T_Parameter& value)
{
    if(_instance._luaparser) {
        lua_State* L = _instance._luaparser->getLuaState();
        lua_pushstring(L, Naming::Lua_reg_references);
        lua_rawget(L, LUA_REGISTRYINDEX);
        deleteSingle(L, value);
        lua_pop(L, 1);
    }
}

bool ParameterTypeSystem::pushValue(lua_State* const L, const struct T_Parameter& value)
{
    if(value.type >= 0 && (unsigned)value.type < _instance._typeList->size())
    {
        (*_instance._typeList)[value.type]->pushValue(L, value.value);
        return true;
    }
    return false;
}

bool ParameterTypeSystem::pushValue(const struct T_Parameter& value)
{
    if(_instance._luaparser) {
        lua_State* L = _instance._luaparser->getLuaState();
        switch(value.type){
        case pid_nil:
        case pid_real:
        case pid_boolean:
        case pid_string:
        case pid_func:
            {
                (*_instance._typeList)[value.type]->pushValue(L, value.value);
                break;
            }
        default:
            {
                lua_pushstring(L, Naming::Lua_reg_references);
                lua_rawget(L, LUA_REGISTRYINDEX);
                if(!lua_rawgetp(L, -1, value.value)) {
                    lua_pop(L, 1);
                    (*_instance._typeList)[value.type]->pushValue(L, value.value);
                    std::string meta("meta:");
                    meta += getParameterName(value.type);
                    luaL_setmetatable(L, meta.c_str());
                    lua_pushvalue(L, -1);
                    lua_rawsetp(L, -3, value.value);
                }
                lua_insert(L, -2);
                lua_pop(L, 1);
                break;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

void ParameterTypeSystem::connect(IConnectEventHandler* const c)
{
    if(c)
    {
        _instance._luaparser = c;
        /*
        for(auto it = _instance._typeList->begin(); it != _instance._typeList->end(); it++)
        {
            c->notifyLoad(*it);
        }
        */
    }
}

ParameterType const * const ParameterTypeSystem::getParameter(const int& id)
{
    if(id >= 0 && (unsigned)id < _instance._typeList->size())
    {
        if((*_instance._typeList)[id] != nullptr)
        {
            return (*_instance._typeList)[id];
        }
    }
    return nullptr;
}

void ParameterTypeSystem::disconnect()
{

}
