#ifndef PLACEMENT_H
#define PLACEMENT_H

#include "Database.h"
// We declare a Placement every time we are doing merge on a clk net
class Placement
{
public:
    Placement()
    {
        _nodes.resize(1);
        _nodes[0] = nullptr;
    };
    void mainLoop(Database *database);
    void constructDiamond();
    void constructGraph();
    void findMST();
    void mergeFF();
    // methods for design (hyper-graph) construction
    void addNode(Node *node) { _nodes.push_back(node); }
    unsigned getNumNode() { return _nodes.size(); }
    void setNodesize(unsigned size);
    void clearNode();
    // maybe need to modified after i think about it
    void DecreaseKeyMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx, double key);
    pair<Node *, pair<double, Node *>> extractMinMST(vector<pair<Node *, pair<double, Node *>>> &heap);
    void swapNodeMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx1, unsigned idx2);
    // get design property
    Node *node(unsigned nodeId) { return _nodes[nodeId]; }

private:
    NodeList _nodes;
};

#endif // PLACEMENT_H
