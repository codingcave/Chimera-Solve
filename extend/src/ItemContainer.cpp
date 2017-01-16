#include <iostream>
#include <unordered_map>
#include <vector>
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"

ItemContainer::ItemContainer()
{
    _items = new map_t_LuaItem();
}

ItemContainer::~ItemContainer()
{
    truncate();
    delete _items;
}

void ItemContainer::truncate()
{
    //for (auto mIt = _items->begin(); mIt != _items->end(); mIt++)
    //{
    //    ParameterTypeSystem::deleteValue(mIt->second);
    //}
    _items->clear();
}

void ItemContainer::setValue(const std::string& name, const struct T_Parameter& value)
{
    const struct T_Parameter* const old = (*this)[name];
    if(old != nullptr)
    {
        ParameterTypeSystem::deleteValue(*old);
    }
    _items->insert (std::make_pair(name,value));
}

struct T_Parameter const * const ItemContainer::operator[](const std::string& name) const
{
    map_t_LuaItem::const_iterator available = _items->find (name);

    if ( available != _items->end() )
    {
        return &(available->second);
    }
    return nullptr;
}

/*
ItemAssigner& ItemContainer::operator[](const std::string& name)
{
    return *(new ItemAssigner(name, this));
}
*/

map_t_LuaItem::const_iterator ItemContainer::beginItems() const
{
    return _items->begin();
}

map_t_LuaItem::const_iterator ItemContainer::endItems() const
{
    return _items->end();
}
