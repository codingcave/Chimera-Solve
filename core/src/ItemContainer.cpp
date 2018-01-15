#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "StateSynchrony.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"

chimera::ItemContainer::ItemContainer()
{
    _items = new map_t_LuaItem();
}

chimera::ItemContainer::~ItemContainer()
{
    //truncate();
    _items->clear();
    delete _items;
}

/*
void chimera::ItemContainer::truncate()
{
    //for (auto mIt = _items->begin(); mIt != _items->end(); mIt++)
    //{
    //    ParameterTypeSystem::deleteValue(mIt->second);
    //}
    _items->clear();
}
*/

void chimera::ItemContainer::setValue(const std::string& name, ParameterValue& value)
{
    auto it = _items->find(name);
    if(it != _items->end())
    {
        it->second.dispose();
    }
    value.bind(getChimeraSystem());
    _items->insert (std::make_pair(name,value));
}

chimera::ParameterValue chimera::ItemContainer::operator[](const std::string& name) const
{
    map_t_LuaItem::const_iterator available = _items->find (name);

    if ( available != _items->end() )
    {
        return available->second;
    }
    return ParameterValue(getChimeraSystem(), 0, nullptr);
}

/*
ItemAssigner& ItemContainer::operator[](const std::string& name)
{
    return *(new ItemAssigner(name, this));
}
*/

chimera::map_t_LuaItem::const_iterator chimera::ItemContainer::beginItems() const
{
    return _items->begin();
}

chimera::map_t_LuaItem::const_iterator chimera::ItemContainer::endItems() const
{
    return _items->end();
}
