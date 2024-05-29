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
        _neighbor.resize(0);
    };
    Node(FFCell *FF) : _FF(FF) { _neighbor.resize(0); }
    void addNeighborPair(pair<Node *, double> n) { _neighbor.push_back(n); }
    void setNeighborPair(unsigned idx, pair<Node *, double> n) { _neighbor[idx] = n; }
    void setFFinNode(FFCell *f) { _FF = f; } // not sure if it's right
    void updateNeighbornode(unsigned id, Node *n) { _neighbor[id].first = n; }
    void updateNeighborweight(unsigned id, double weight) { _neighbor[id].second = weight; }
    void setNeighborsize(unsigned size) { _neighbor.resize(size); }
    unsigned getNeighborsize() { return _neighbor.size(); }
    pair<Node *, double> getNeighborPair(unsigned idx) { return _neighbor[idx]; }
    // get////////////////////////////////
    Node *getneighborNode(unsigned idx)
    {
        assert(idx < _neighbor.size());
        return _neighbor[idx].first;
    }
    double getneighborweight(unsigned idx)
    {
        assert(idx < _neighbor.size());
        return _neighbor[idx].second;
    }
    FFCell *getFFinNode() { return _FF; }

private:
    vector<pair<Node *, double>> _neighbor;
    FFCell *_FF;
};
#endif