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
#include "ParameterValueCollection.hpp"

chimera::ParameterValueCollection::ParameterValueCollection()
{
    _items = new map_t_LuaItem();
}

chimera::ParameterValueCollection::~ParameterValueCollection()
{
    //truncate();
    _items->clear();
    delete _items;
}

void chimera::ParameterValueCollection::setValue(const std::string& name, ParameterValue& value)
{
    auto it = _items->find(name);
    if(it != _items->end())
    {
        it->second.dispose();
    }
    if (value.getType() != 0) {
        value.bind(getChimeraSystem());
        _items->insert (std::make_pair(name,value));
    }
}

chimera::ParameterValue chimera::ParameterValueCollection::operator[](const std::string& name) const
{
    map_t_LuaItem::const_iterator available = _items->find (name);

    if ( available != _items->end() )
    {
        return available->second;
    }
    return ParameterValue(getChimeraSystem(), 0, nullptr);
}

chimera::map_t_LuaItem::const_iterator chimera::ParameterValueCollection::beginItems() const
{
    return _items->begin();
}

chimera::map_t_LuaItem::const_iterator chimera::ParameterValueCollection::endItems() const
{
    return _items->end();
}
