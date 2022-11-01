#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "ChimeraContext.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"

chimera::ParameterValueCollection::ParameterValueCollection()
{
    _items = new map_t_LuaItem();
}

chimera::ParameterValueCollection::~ParameterValueCollection()
{
    _items->clear();
    delete _items;
}

void chimera::ParameterValueCollection::setValue(const std::string& name, ParameterValue& value)
{
    // delete old value if it exists
    auto it = _items->find(name);
    if(it != _items->end())
    {
        it->second.dispose();
    }
    if (value.getType() != 0 && _context != nullptr) {
        value.bind(_context->_chimeraSystem);
        _items->insert (std::make_pair(name,value));
    }
}

chimera::ParameterValue chimera::ParameterValueCollection::operator[](const std::string& name) const
{
    map_t_LuaItem::const_iterator available = _items->find (name);
    // return value if it exists
    if ( available != _items->end() )
    {
        return available->second;
    }
    // return nil otherwise
    return ParameterValue(_context->_chimeraSystem, 0, nullptr);
}

chimera::map_t_LuaItem::const_iterator chimera::ParameterValueCollection::beginItems() const
{
    return _items->begin();
}

chimera::map_t_LuaItem::const_iterator chimera::ParameterValueCollection::endItems() const
{
    return _items->end();
}
