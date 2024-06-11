#ifndef PLACEMENT_H
#define PLACEMENT_H

#include "Database.h"
// We declare a Placement every time we are doing merge on a clk net
class Placement {
   public:
    Placement() {
        _nodes.resize(1);
        _nodes[0] = nullptr;
    };
    void mainLoop();
    void constructDiamond();
    void constructGraph();
    NodeList findMST();
    // methods for design (hyper-graph) construction
    void addNode(Node *node) { _nodes.push_back(node); }
    unsigned getNumNode() { return _nodes.size(); }
    void setNodesize(unsigned size);
    void clearNode();
    // func for MST //////////////
    void DecreaseKeyMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx, double key);
    pair<Node *, pair<double, Node *>> extractMinMST(vector<pair<Node *, pair<double, Node *>>> &heap);
    void swapNodeMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx1, unsigned idx2);
    // func for MST //////////////
    // func for mergeFF////////////
    void mergeFFinG();
    void merge2FF(unsigned idx1, unsigned idx2, unsigned newffidx);
    // get design property
    Node *node(unsigned nodeId) { return _nodes[nodeId]; }
    void setDatabase(Database *dataBase) { _dataBase = dataBase; }
    Database *getDatabase() { return _dataBase; }

   private:
    Database *_dataBase;
    // construct graph
    vector<Module *> _diamondINF;  // Graph_input
    NodeList _nodes;
    map<string, Node *> _name2Node;
};

#endif  // PLACEMENT_H
