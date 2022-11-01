#ifndef MAPPER_H
#define MAPPER_H

#include <QtCore/QMap>
#include <QtCore/QString>

class Subcircuit;
class Circuit;

class Mapper
{
public:
    virtual ~Mapper(){}

    unsigned index(const QString& node,bool* exists);
    void mapper(const QString& node,unsigned* indx);

    virtual Subcircuit* findSubcircuit(const QString& name) const = 0;
    virtual Circuit* getRoot() = 0;
    virtual Mapper* getParent() = 0;

protected:
    QMap<QString,unsigned> m_map;
};

#endif // MAPPER_H
