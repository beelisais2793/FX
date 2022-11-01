#include "primitive.h"
#include "mapper.h"
#include "circuit.h"
#include "dbg.h"

void Primitive::map(Mapper *parent)
{
    setParent(parent);
    Circuit* r = root();
    if(!r)
    {
        qFatal("NULL root detected!\n");
        return;
    }

    unsigned n = r->N();

    for(int i = 0; i<nodes().size(); ++i)
    {
        parent->mapper(nodes().at(i),&n);
    }

    r->N() = n;

    switch(name().at(0).toUpper().toLatin1())
    {
      case 'V':
      case 'E':
      case 'H':
        r->M() += 1;
        break;
    }
}
