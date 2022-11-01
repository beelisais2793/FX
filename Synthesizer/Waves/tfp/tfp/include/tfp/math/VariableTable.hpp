#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include <vector>
#include <unordered_map>
#include <set>

namespace tfp {

class Expression;

/*!
 * @brief Maps symbols to values, or to further expressions.
 *
 * When an expression is evaluated, it uses the variable table to determine the
 * values of its symbols.
 */
class TFP_PUBLIC_API VariableTable : public tfp::RefCounted
{
public:
    typedef std::vector< std::pair<std::string, double> > ValueVariables;
    typedef std::vector< std::pair<std::string, std::string> > ExpressionVariables;
    
    struct MissingEntryException : public std::logic_error
    {
        MissingEntryException(const char* key) : std::logic_error(
            std::string("Entry \"") + key + "\" does not exist in variable table"
        ) {}
    };
    
    struct CyclicDependencyException : public std::logic_error
    {
        CyclicDependencyException(const char* key) : std::logic_error(
            std::string("Cyclic dependency of entries was detected while looking up key \"") + key + "\""
        ) {}
    };

    /*!
     * @brief Adds a new entry to the table that maps "name" to the constant of
     * "value". If the entry already exists, then the existing entry is
     * modified.
     * @note The value is stored as a constant expression (Expression::make(value)).
     */
    void set(std::string name, double value);

    /*!
     * @brief Adds a new entry to the table that maps "name" to an expression.
     * If the entry already exists, then the existing entry is replaced.
     */
    void set(std::string name, const char* expression);

    /*!
     * @brief Retrieves the expression the specified symbol would be mapped to,
     * or NULL if no entry exists.
     */
    Expression* get(const std::string& name) const;

    /*!
     * @brief Removes the variable from the table. If it doesn't exist, nothing
     * happens.
     */
    void remove(std::string name);

    /*!
     * @brief Removes all entries from the table.
     */
    void clear();

    /*!
     * @brief Merges variables from another table. If variables in the other
     * table already exist in this table, then they are overwritten.
     */
    void merge(const VariableTable& source);

    /*!
     * @brief Recursively evaluates the expression the specified variable maps
     * to until a constant value is computed.
     */
    double valueOf(std::string name) const;
    double valueOf(std::string name, std::set<std::string>* visited) const;

    /*!
     * @brief Generates an alphabetically sorted list of all entries that have
     * direct values.
     */
    ValueVariables valueVariableList() const;

    /*!
     * @brief Generates an alphabetically sorted list of all entries that have
     * further expressions as their value.
     */
    ExpressionVariables expressionVariableList() const;

private:
    typedef std::unordered_map<std::string, tfp::Reference<Expression> > Table;
    Table table_;
};

}
