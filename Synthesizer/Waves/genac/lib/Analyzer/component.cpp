#include "component.h"
#include "circuit.h"
#include "mapper.h"
#include "dbg.h"

Circuit* Component::root()
{
    Mapper *p = this->parent();
    while(p->getParent())
        p = p->getParent();

    return dynamic_cast<Circuit*>(p);
}
