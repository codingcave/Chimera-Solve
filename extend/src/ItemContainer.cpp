#include <iostream>
#include <unordered_map>
#include <vector>
#include "ParameterValue.hpp"
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

void ItemContainer::setValue(const std::string& name, ParameterValue& value)
{
    auto it = _items->find(name);
    if(it != _items->end())
    {
        it->second.dispose();
    }
    _items->insert (std::make_pair(name,value));
}

ParameterValue ItemContainer::operator[](const std::string& name) const
{
    map_t_LuaItem::const_iterator available = _items->find (name);

    if ( available != _items->end() )
    {
        return available->second;
    }
    return ParameterValue(0, nullptr);
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
