#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "types/lua_basetypes.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
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

chimera::ParameterTypeSystem::ParameterTypeSystem():
    _chimeraSystem(nullptr),
    _lastID(7),
    _typeList(new std::vector<chimera::ParameterType*>(8)),
    _references(new std::unordered_map<void*, chimera::ParameterValue>()),
    _dependencies(new std::unordered_map<void*, std::unordered_set<void*> >()),
    _reverseDependencies(new std::unordered_map<void*, std::unordered_set<void*> >())
{
    (*_typeList)[chimera::systemtypes::PID_NIL] = new ParameterType(chimera::typenames::TYPE_NIL, chimera::systemtypes::PID_NIL, this, {nullptr, chimera::luat_nil_push, nullptr, nullptr});
    (*_typeList)[chimera::systemtypes::PID_NUMBER] = new ParameterType(chimera::typenames::TYPE_NUMBER, chimera::systemtypes::PID_NUMBER, this, {nullptr, chimera::luat_real_push, chimera::luat_real_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_BOOLEAN] = new ParameterType(chimera::typenames::TYPE_BOOLEAN, chimera::systemtypes::PID_BOOLEAN, this, {nullptr, chimera::luat_boolean_push, chimera::luat_boolean_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_STRING] = new ParameterType(chimera::typenames::TYPE_STRING, chimera::systemtypes::PID_STRING, this, {nullptr, chimera::luat_string_push, chimera::luat_string_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_TABLE] = new ParameterType(chimera::typenames::TYPE_TABLE, chimera::systemtypes::PID_TABLE, this, {nullptr, chimera::luat_table_push, chimera::luat_table_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_POINTER] = new ParameterType(chimera::typenames::TYPE_POINTER, chimera::systemtypes::PID_POINTER, this, {nullptr, chimera::luat_pointer_push, nullptr, nullptr});
    (*_typeList)[chimera::systemtypes::PID_FUNCTION] = new ParameterType(chimera::typenames::TYPE_FUNCTION, chimera::systemtypes::PID_FUNCTION, this, {nullptr, chimera::luat_function_push, nullptr, nullptr});
    (*_typeList)[chimera::systemtypes::PID_LIBRARAY] = new ParameterType(chimera::typenames::TYPE_LIBRARY, chimera::systemtypes::PID_LIBRARAY, this, {nullptr, nullptr, nullptr, nullptr});
    registerParameter(chimera::typenames::TYPE_USERFUNCTION, this, {chimera::luat_luafunction_init, chimera::luat_function_push, chimera::luat_luafunction_delete, nullptr});
    registerParameter(chimera::typenames::TYPE_ENTRYPOINT, this, {nullptr, nullptr, nullptr, nullptr});
    registerParameter(chimera::typenames::TYPE_MODULE, this, {nullptr, nullptr, nullptr, nullptr});
    registerParameter(chimera::typenames::TYPE_INSTANCE, this, {nullptr, nullptr, nullptr, nullptr});
    stateLoaded();
}

chimera::ParameterTypeSystem::~ParameterTypeSystem()
{
    _references->clear();
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        itemRemoved(*it, nullptr);
        delete *it;
    }
    delete _typeList;
    delete _references;
    delete _dependencies;
    delete _reverseDependencies;
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct chimera::T_ParameterDef& pdef)
{
    return registerParameter(name, origin, pdef, 0, 0, std::unordered_map<std::string, size_t>(), true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag)
{
    return registerParameter(name, origin, pdef, base, tag, std::unordered_map<std::string, size_t>(), true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct chimera::T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& flags)
{
    return registerParameter(name, origin, pdef, 0, 0, flags, true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags)
{
    return registerParameter(name, origin, pdef, base, tag, flags, true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& flags, bool notify)
{
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        if( (*it)->getName() == name )
        {
            return 0;
        }
    }
    ++_lastID;
    ParameterType* p = new ParameterType(name, _lastID, origin, pdef, base, tag, flags);
    _typeList->push_back(p);
    if(notify) {
        itemAdded(p, nullptr);
    }
    return _lastID;
}

void chimera::ParameterTypeSystem::removeParameter(const std::string& name)
{
    removeParameter(getParameterID(name));
}

void chimera::ParameterTypeSystem::removeParameter(const size_t& id)
{
    if(id > chimera::systemtypes::PID_INSTANCE && id < _typeList->size())
    {
        const ParameterType* p = (*_typeList)[id];
        if(p != nullptr) {
            itemRemoved(p, nullptr);
            deleteAllReferences(id);
            p->dispose(_chimeraSystem->getLuaState());
            delete p;
            (*_typeList)[id] = nullptr;
        }
    }
}

void chimera::ParameterTypeSystem::removeParameter(void const * const origin)
{
    const ParameterType* p = nullptr;
    for(size_t id = chimera::systemtypes::PID_INSTANCE + 1; id < _typeList->size(); id++)
    {
        p = (*_typeList)[id];
        if(p != nullptr && p->_origin == origin)
        {
            itemRemoved(p, nullptr);
            deleteAllReferences(id);
            delete p;
            (*_typeList)[id] = nullptr;
        }
    }
}

void chimera::ParameterTypeSystem::deleteAllReferences(size_t type)
{
    for(auto it = _references->begin(); it != _references->end(); it++)
    {
        if(it->second.getType() == type) {
            deleteValue(_chimeraSystem->getLuaState(), it->second);
            it->second._data->type = 0;
        }
    }
}

size_t chimera::ParameterTypeSystem::getParameterID(const std::string& name) const
{
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        if( (*it)->getName() == name )
        {
            return (*it)->getID();
        }
    }
    return 0;
}

const bool chimera::ParameterTypeSystem::exists(const size_t& id) const
{
    if(id < _typeList->size())
    {
        const ParameterType* p = (*_typeList)[id];
        if(p != nullptr) {
            return true;
        }
    }
    return false;
}

/*
size_t ParameterTypeSystem::getParameterID(const size_t& base, const size_t& tag)
{
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        if( (*it)->getBase() == base && (*it)->getTag() == tag )
        {
            return (*it)->getID();
        }
    }
    return 0;
}
*/

chimera::ParameterValue chimera::ParameterTypeSystem::getValue(lua_State* const L, const int& index)
{
    switch(lua_type(L, index))
    {
    case LUA_TNONE:
    case LUA_TNIL:
        {
            return ParameterValue(chimera::systemtypes::PID_NIL, nullptr);
        }
    case LUA_TNUMBER:
        {
            return ParameterValue(_chimeraSystem, chimera::systemtypes::PID_NUMBER, new double(lua_tonumber(L, index)));
        }
    case LUA_TBOOLEAN:
        {
            return ParameterValue(_chimeraSystem, chimera::systemtypes::PID_BOOLEAN, new bool(lua_toboolean(L, index)));
        }
    case LUA_TSTRING:
        {
            return ParameterValue(_chimeraSystem, chimera::systemtypes::PID_STRING, new std::string(lua_tostring(L, index)));
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
                            table->insert(std::make_pair(key, getValue(L, -1)));
                            //(*table)[key] = value;
                        }
                        break;
                    }
                case LUA_TSTRING:
                    {
                        std::string key(lua_tostring(L, -2));
                        table->insert(std::make_pair(key, getValue(L, -1)));
                        //(*table)[key] = value;
                        break;
                    }
                }

                lua_pop(L, 1);
            }

            return ParameterValue(_chimeraSystem, chimera::systemtypes::PID_TABLE, table);
        }
    case LUA_TFUNCTION:
        {
            int f_ind = lua_absindex(L, index);
            lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
            lua_rawget(L, LUA_REGISTRYINDEX);
            void* w = (void*)(new chimera::LuaFunctionWrapper(_chimeraSystem));
            lua_pushvalue(L, f_ind);
            lua_rawsetp(L, -2, w);
            lua_pop(L, 1);

            return ParameterValue(_chimeraSystem, chimera::systemtypes::PID_LUAFUNC, w);
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
                    if(type > 0 && (unsigned)type >= chimera::systemtypes::PID_MODULE && (*_typeList)[type] != nullptr)
                    {
                        void* value = *((void**)lua_touserdata(L, f_ind));
                        /*
                        lua_pushstring(L, Naming::Lua_reg_references);
                        lua_rawget(L, LUA_REGISTRYINDEX);
                        lua_pushvalue(L, f_ind);
                        lua_rawsetp(L, -2, value);
                        lua_pop(L, 1);
                        return {type, value};
                        */
                        ParameterValue pv(_chimeraSystem, type, value);
                        addDependency(L, value);

                        return pv;
                    }
                    else
                    {
                        return ParameterValue(type, nullptr);
                    }
                }
            }
            break;
        }
    case LUA_TTHREAD:
        {

            break;
        }
    case LUA_TLIGHTUSERDATA:
        {
            return ParameterValue(_chimeraSystem, chimera::systemtypes::PID_POINTER, lua_touserdata(L, index));
            break;
        }
    }

    return ParameterValue(chimera::systemtypes::PID_NIL, nullptr);
}

size_t chimera::ParameterTypeSystem::getParameterType(lua_State* const L, const int& index) const
{
    return ParameterType::getType(L, index);
}

const std::string chimera::ParameterTypeSystem::getParameterName(const size_t& id) const
{
    if(id >= 0 && id < _typeList->size())
    {
        if((*_typeList)[id] != nullptr)
        {
            return (*_typeList)[id]->getName();
        }
    }
    return "";
}

size_t chimera::ParameterTypeSystem::getParameterBase(const size_t& id) const
{
    if(id >= 0 && id < _typeList->size())
    {
        if((*_typeList)[id] != nullptr)
        {
            return (*_typeList)[id]->getBase();
        }
    }
    return 0;
}

size_t chimera::ParameterTypeSystem::getParameterTag(const size_t& id) const
{
    if(id >= 0 && id < _typeList->size())
    {
        if((*_typeList)[id] != nullptr)
        {
            return (*_typeList)[id]->getTag();
        }
    }
    return 0;
}

void chimera::ParameterTypeSystem::deleteValue(lua_State* const L, chimera::ParameterValue& value) const
{
    if(value.getType() >= 0 && value.getType() < _typeList->size())
    {
        (*_typeList)[value.getType()]->deleteValue(L, value.getValue());
    }
}

void chimera::ParameterTypeSystem::deleteValue(lua_State* const L, size_t type, void* value) const
{
    if(type >= 0 && type < _typeList->size())
    {
        (*_typeList)[type]->deleteValue(L, value);
    }
}

bool chimera::ParameterTypeSystem::pushValue(lua_State* const L, struct chimera::T_Parameter value)
{
    return pushValue(L, value.type, value.value);
}

bool chimera::ParameterTypeSystem::pushValue(lua_State* const L, size_t type, void* value)
{
    switch(type) {
    case chimera::systemtypes::PID_NIL:
    case chimera::systemtypes::PID_NUMBER:
    case chimera::systemtypes::PID_BOOLEAN:
    case chimera::systemtypes::PID_STRING:
    case chimera::systemtypes::PID_TABLE:
    case chimera::systemtypes::PID_FUNCTION:
    case chimera::systemtypes::PID_POINTER:
        {
            (*_typeList)[type]->pushValue(L, value);
            break;
        }
    case chimera::systemtypes::PID_LUAFUNC:
        {
            LuaFunctionWrapper* fw = (LuaFunctionWrapper*)value;
            if(fw->intern())
            {
                (*_typeList)[type]->pushValue(L, (void*)fw->getOrigin());

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_rawgeti(L, -1, type);
                lua_setmetatable(L, -2);
                lua_pop(L, 1);

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_pushvalue(L, -2);
                lua_rawsetp(L, -2, (void*)fw->getOrigin());
                lua_pop(L, 1);

                auto it = _references->find((void*)fw->getOrigin());
                if(it != _references->end())
                {
                    addDependency(fw->_chSys->getLuaState(), value);
                }
            }
            else
            {
                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_pushlightuserdata(L, (void*)fw->getOrigin());
                lua_rawget(L, -2);
                lua_remove(L, -2);
            }
            break;
        }
    default:
        {
            if(type > 0 && type < _typeList->size())
            {
                auto it = _references->find(value);

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                if(it != _references->end())
                {
                    if(_reverseDependencies->count(value))
                    {
                        if(lua_rawgetp(L, -1, value))
                        {
                            lua_remove(L, -2);
                            return true;
                        }
                        lua_pop(L, 1);
                    }
                    addDependency(_chimeraSystem->getLuaState(), value);
                } else {
                    ParameterValue p(_chimeraSystem, type, value);
                    addDependency(_chimeraSystem->getLuaState(), value);
                    p.dispose();
                }
                (*_typeList)[type]->pushValue(L, value);
                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_rawgeti(L, -1, type);
                lua_setmetatable(L, -3);
                lua_pop(L, 1);

                lua_pushvalue(L, -1);
                lua_rawsetp(L, -3, value);
                lua_remove(L, -2);
            }
            else
            {
                return false;
            }
            break;
        }
    }
    return true;
}

chimera::ParameterType const * const chimera::ParameterTypeSystem::getParameter(const size_t& id) const
{
    if(id >= 0 && id < _typeList->size())
    {
        if((*_typeList)[id] != nullptr)
        {
            return (*_typeList)[id];
        }
    }
    return nullptr;
}

bool chimera::ParameterTypeSystem::isReferenced(void* ptr) const
{
    auto it = _references->find(ptr);
    if(it != _references->end())
    {
        return it->second.getType() > 0;
    }
    return false;
}

chimera::ParameterValue chimera::ParameterTypeSystem::createValue(size_t type, void* value) const
{
    return ParameterValue(_chimeraSystem, type, value);
}

chimera::LuaFunctionWrapper chimera::ParameterTypeSystem::createFunction(chimera::fn_luafnwrapper fn) const
{
    return LuaFunctionWrapper(_chimeraSystem, fn);
}

void chimera::ParameterTypeSystem::addDependency(void* item, void* dependency)
{
    {
        if (!_dependencies->count(item))
        {
            _dependencies->insert(std::make_pair(item, std::unordered_set<void*>()));
        }

        auto listItem = _dependencies->find(item);
        if(listItem != _dependencies->end())
        {
            if(!listItem->second.count(dependency))
            {
                listItem->second.insert(dependency);
            }
        }
    }

    {
        if (!_reverseDependencies->count(dependency))
        {
            _reverseDependencies->insert(std::make_pair(dependency, std::unordered_set<void*>()));
        }

        auto listItem = _reverseDependencies->find(dependency);
        if(listItem != _reverseDependencies->end())
        {
            if(!listItem->second.count(item))
            {
                listItem->second.insert(item);
            }
        }
    }
}

void chimera::ParameterTypeSystem::releaseDependency(void* item, void* dependency)
{
    auto listItem = _dependencies->find(item);
    auto listDep = _reverseDependencies->find(dependency);
    if (listItem != _dependencies->end() && listDep != _reverseDependencies->end())
    {
        listItem->second.erase(dependency);
        listDep->second.erase(item);

        if (listDep->second.size() == 0)
        {
            _reverseDependencies->erase(listDep);
            removeDependencyItem(dependency);
            disposeReference(dependency);
        }

        if (listItem->second.size() == 0)
        {
            _dependencies->erase(listItem);
        }
    }
}

void chimera::ParameterTypeSystem::removeDependencyItem(void* item)
{
    //static void* dependency;
    std::unordered_set<void*> itemCache;

    auto depList = _dependencies->find(item);
    if(depList != _dependencies->end())
    {
        for(auto it = depList->second.begin(); it != depList->second.end(); it++)
        {
            auto itemList = _reverseDependencies->find(*it);
            if (itemList != _reverseDependencies->end())
            {
                itemList->second.erase(item);
                if (itemList->second.size() == 0)
                {
                    _reverseDependencies->erase(itemList);
                    // delete reference
                    itemCache.insert(*it);
                }
            }
        }
        _dependencies->erase(depList);

        for(auto it = itemCache.begin(); it != itemCache.end(); it++)
        {
            removeDependencyItem(*it);
            disposeReference(*it);
        }
    }
}

void chimera::ParameterTypeSystem::disposeReference(void* value)
{
    auto refItem = _references->find(value);
    if(refItem != _references->end()) {
        if(refItem->second._data->references == 0 && !_reverseDependencies->count(value))
        {
            deleteValue(_chimeraSystem->getLuaState(), refItem->second._data->type, refItem->second._data->value);
            refItem->second.dispose();
            _references->erase(value);
        }
    }
}

/*
void chimera::ParameterTypeSystem::notifyUnload(StateSynchrony* sender, void const * const data)
{
    removeParameter(sender);
}

void chimera::ParameterTypeSystem::notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data)
{
    removeParameter(item);
}
*/

const std::string chimera::ParameterTypeSystem::getGUID() const
{
    return "ParameterTypeSystem";
}
