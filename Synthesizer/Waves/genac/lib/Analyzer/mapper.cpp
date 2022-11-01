#include "mapper.h"
#include "dbg.h"

unsigned Mapper::index(const QString &node, bool *exists)
{
    if(!m_map.contains(node))
    {
        *exists = false;
        return 0;
    }

    *exists = true;
    return m_map.value(node);
}

void Mapper::mapper(const QString &node, unsigned* indx)
{
    if(!m_map.contains(node))
    {
        if(node != "0")
            m_map.insert(node,++(*indx));
        else
            m_map.insert(node,0);
    }
}
