#pragma once 

#include <string>

namespace Std {

class StdObject
{
public:

    virtual ~StdObject() {} 

    virtual std::string name() const { return "StdObject"; }

    virtual void save(std::string& filename) {}
    virtual void load(std::string& filename) {}

    virtual std::string str() const { return ""; }
    virtual void* ptr() const { return NULL; }
 };

}
