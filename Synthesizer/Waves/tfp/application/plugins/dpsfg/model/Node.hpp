#pragma once

#include "tfp/util/Reference.hpp"
#include <string>
#include <vector>
#include <memory>

namespace dpsfg {

class Connection;

class Node
{
public:
    typedef std::vector< std::unique_ptr<Connection> > ConnectionList;
    Node();
    Node(const std::string& name);

    /*!
     * Creates a new connection between this node and another node. The connection
     * is out-going (direction matters).
     */
    Connection* connectTo(Node* node);

    /*!
     * Creates a new connection between another node and this node. The connection
     * is incoming (direction matters).
     */
    Connection* connectFrom(Node* node);

    /*!
     * Disconnects this node from the specified node, regardless of connection
     * direction and regardless of how many connections exist between the two
     * nodes. (Destroys as many connections as necessary to disconnect these
     * nodes)
     */
    void disconnectFrom(Node* node);

    /*!
     * Disconnects all outgoing connections to the specified node. The other
     * node may still have incoming connections after this call returns.
     */
    void disconnectOutgoing(Node* node);

    const ConnectionList& getOutgoingConnections() const;

    std::string getName() const;
    void setName(std::string name);

    std::string getPhysicalUnit() const;
    void setPhysicalUnit(std::string unit);

private:
    std::string name_;
    std::string physicalUnit_;
    ConnectionList outgoingConnections_;
};

}
