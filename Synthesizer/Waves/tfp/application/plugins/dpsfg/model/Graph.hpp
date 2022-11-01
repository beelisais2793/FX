#pragma once

#include "tfp/util/Reference.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <unordered_map>
#include <memory>

namespace dpsfg {
class Node;
class Connection;

class Graph : public tfp::RefCounted
{
public:
    struct PathSegment
    {
        Node* in_;
        Node* out_;
        Connection* connection_;
    };

    typedef std::vector<PathSegment> Path;
    typedef std::vector<Path> PathList;
    typedef std::vector<Node*> NodeList;

    Graph();

    Node* createNode(const char* name);
    Node* findNode(const char* name);

    void setForwardPath(Node* in, Node* out);
    tfp::VariableTable* variables();

    tfp::Expression* mason() const;
    tfp::TransferFunction<double> calculateTransferFunction() const;

    bool evaluatePhysicalUnitConsistencies() const;

    void findForwardPathsAndLoops(PathList* paths, PathList* loops) const;
    void findForwardPathsAndLoopsRecursive(PathList* paths, PathList* loops,
                                           Node* current, NodeList list) const;
    void nodeListToPath(Path* path, const NodeList& nodes) const;
    tfp::Expression* calculateConnectionGain(const Path& path) const;
    tfp::Expression* calculateDeterminant(const PathList& loops) const;
    tfp::Expression* calculateCofactorsAndPathGains(const PathList& loops, const PathList& paths) const;
    bool pathsAreTouching(const Path& a, const Path& b) const;

    void dump(const char* fileName) const;
    void dump(FILE* fileName) const;

private:
    Node* input_;
    Node* output_;
    std::unordered_map< std::string, std::unique_ptr<Node> > nodes_;
    tfp::Reference<tfp::VariableTable> variables_;
};

} // namespace tfp
