#include "tfp/math/VariableTable.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
void VariableTable::set(std::string name, double value)
{
    Table::const_iterator it = table_.find(name);
    if (it == table_.end())
        table_[name] = Expression::make(value);
    else
        it->second->set(value);
}

// ----------------------------------------------------------------------------
void VariableTable::set(std::string name, const char* expression)
{
    table_[name] = Expression::parse(expression);
}

// ----------------------------------------------------------------------------
Expression* VariableTable::get(const std::string& name) const
{
    Table::const_iterator it = table_.find(name);
    if (it == table_.end())
        return NULL;
    return it->second;
}

// ----------------------------------------------------------------------------
void VariableTable::remove(std::string name)
{
    table_.erase(name);
}

// ----------------------------------------------------------------------------
void VariableTable::clear()
{
    table_.clear();
}

// ----------------------------------------------------------------------------
void VariableTable::merge(const VariableTable& other)
{
    table_.insert(other.table_.begin(), other.table_.end());
}

// ----------------------------------------------------------------------------
double VariableTable::valueOf(std::string name) const
{
    std::set<std::string> visited;
    return valueOf(name, &visited);
}

// ----------------------------------------------------------------------------
double VariableTable::valueOf(std::string name, std::set<std::string>* visited) const
{
    Expression* e = get(name);
    if (e == NULL)
        throw MissingEntryException(name.c_str());
    if (e->type() == Expression::VARIABLE && visited->insert(name).second == false)
        throw CyclicDependencyException(name.c_str());
    return e->evaluate(this, visited);
}
