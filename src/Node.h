#ifndef NODE_H
#define NODE_H
#include <vector>
#include <utility>
#include "Module.h"
#include "DatabaseDef.h"
using namespace std;

class Node
{
public:
    Node() : _FF(nullptr)
    {
        _neighbor_map.clear();
    };
    Node(Module *FF) : _FF(FF) { _neighbor_map.clear(); }
    void addNeighborPair(pair<Node *, double> n) { _neighbor_map.insert(make_pair(n.first->getFFinNode()->name(), n)); }
    void setNeighborPair(string idx, pair<Node *, double> n) { _neighbor_map[idx] = n; }
    void setFFinNode(Module *f) { _FF = f; } // not sure if it's right
    void updateNeighbornode(string id, Node *n) { _neighbor_map[id].first = n; }
    void updateNeighborweight(string id, double weight) { _neighbor_map[id].second = weight; }
    void setisleaf(bool b) { _isleaf = b; }
    // void setNeighborsize(unsigned size) { _neighbor.resize(size); }
    void clearNeighbor() { _neighbor_map.clear(); }
    void setNodeidxheap(unsigned id) { _heapidx = id; }
    unsigned getNodeidxheap() { return _heapidx; }
    unsigned getNeighborsize() { return _neighbor_map.size(); }
    bool getisleaf() { return _isleaf; }
    pair<Node *, double> getNeighborPair(string idx) { return _neighbor_map[idx]; }
    // get////////////////////////////////
    Node *getneighborNode(string idx)
    {
        assert(_neighbor_map.count(idx));
        return _neighbor_map[idx].first;
    }
    double getneighborweight(string idx)
    {
        assert(_neighbor_map.count(idx));
        return _neighbor_map[idx].second;
    }
    map<string, pair<Node *, double>> getneighbormap() { return _neighbor_map; }
    void eraseNeighbor(string idx) { _neighbor_map.erase(idx); }
    Module *getFFinNode() { return _FF; }

private:
    map<string, pair<Node *, double>> _neighbor_map;
    Module *_FF;
    unsigned _heapidx; // forMST
    bool _isleaf;      // forMST
};
#endif