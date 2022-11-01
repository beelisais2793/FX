#ifndef MAPABLE_H
#define MAPABLE_H

class Mapper;
class Analyzer;

//!Interface for the objects to be mapped and stamped.
class Mapable
{
public:
    virtual ~Mapable(){}
    virtual void map(Mapper* parent) = 0;
    virtual void stamp(Analyzer* a) = 0;
};

#endif
