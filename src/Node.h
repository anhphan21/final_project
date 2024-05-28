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
    Node();
    Node(FFCell *FF) : _FF(FF) { _neighbor.resize(0); }
    void addNeighborPair(pair<Node *, double> n) { _neighbor.push_back(n); }
    void setFF(FFCell *f) { _FF = f; } // not sure if it's right
    ////////////////////////////////
    // I think to use this function:
    // FFCell f1;
    // setFF(&f1);
    ////////////////////////////////
    void updateNeighbornode(unsigned id, Node *n) { _neighbor[id].first = n; }
    void updateNeighborweight(unsigned id, double weight) { _neighbor[id].second = weight; }
    void setNeighborsize(unsigned size) { _neighbor.resize(size); }
    // get////////////////////////////////
    Node &neighborNode(unsigned idx)
    {
        assert(idx < _neighbor.size());
        return *_neighbor[idx].first;
    }
    double neighborweight(unsigned idx)
    {
        assert(idx < _neighbor.size());
        return _neighbor[idx].second;
    }
    FFCell &ff() { return *_FF; }

private:
    vector<pair<Node *, double>> _neighbor;
    FFCell *_FF;
};
#endif