#include "Placement.h"
#include <vector>
#include <limits>
using namespace std;
void Placement::setNodesize(unsigned size)
{
    clearNode();
    for (size_t i = 0; i < size; i++)
    {
        Node *node = new Node();
        _nodes.push_back(node);
    }
    return;
}
void Placement::clearNode()
{
    for (unsigned i = 0; i < _nodes.size(); i++)
    {
        free(_nodes[i]);
    }
    _nodes.clear();
    return;
}
void mainLoop(Database *database)
{
    // find same clk path-> calculate diamond -> construct graph according to cost -> find MST -> merge FFs
}
void Placement::findMST()
{
    // Testcase for MST//////////////////////////////////////////
    setNodesize(9);
    pair<Node *, double> p;
    vector<FFCell *> j;
    j.clear();
    string name = "a";
    for (size_t i = 0; i < 9; i++)
    {
        FFCell *ff = new FFCell();
        j.push_back(ff);
    }
    name = "a";
    j[0]->setName(name);
    name = "b";
    j[1]->setName(name);
    name = "c";
    j[2]->setName(name);
    name = "d";
    j[3]->setName(name);
    name = "e";
    j[4]->setName(name);
    name = "f";
    j[5]->setName(name);
    name = "g";
    j[6]->setName(name);
    name = "h";
    j[7]->setName(name);
    name = "i";
    j[8]->setName(name);
    for (size_t i = 0; i < 9; i++)
    {
        _nodes[i]->setFFinNode(j[i]);
    }
    p.first = _nodes[1];
    p.second = 4;
    _nodes[0]->addNeighborPair(p);
    p.first = _nodes[7];
    p.second = 8;
    _nodes[0]->addNeighborPair(p);
    p.first = _nodes[0];
    p.second = 4;
    _nodes[1]->addNeighborPair(p);
    p.first = _nodes[7];
    p.second = 11;
    _nodes[1]->addNeighborPair(p);
    p.first = _nodes[2];
    p.second = 8;
    _nodes[1]->addNeighborPair(p);
    p.first = _nodes[1];
    p.second = 8;
    _nodes[2]->addNeighborPair(p);
    p.first = _nodes[8];
    p.second = 2;
    _nodes[2]->addNeighborPair(p);
    p.first = _nodes[3];
    p.second = 7;
    _nodes[2]->addNeighborPair(p);
    p.first = _nodes[5];
    p.second = 4;
    _nodes[2]->addNeighborPair(p);
    p.first = _nodes[2];
    p.second = 7;
    _nodes[3]->addNeighborPair(p);
    p.first = _nodes[4];
    p.second = 9;
    _nodes[3]->addNeighborPair(p);
    p.first = _nodes[5];
    p.second = 14;
    _nodes[3]->addNeighborPair(p);
    p.first = _nodes[3];
    p.second = 9;
    _nodes[4]->addNeighborPair(p);
    p.first = _nodes[5];
    p.second = 10;
    _nodes[4]->addNeighborPair(p);
    p.first = _nodes[4];
    p.second = 10;
    _nodes[5]->addNeighborPair(p);
    p.first = _nodes[3];
    p.second = 14;
    _nodes[5]->addNeighborPair(p);
    p.first = _nodes[2];
    p.second = 4;
    _nodes[5]->addNeighborPair(p);
    p.first = _nodes[6];
    p.second = 2;
    _nodes[5]->addNeighborPair(p);
    p.first = _nodes[5];
    p.second = 2;
    _nodes[6]->addNeighborPair(p);
    p.first = _nodes[8];
    p.second = 6;
    _nodes[6]->addNeighborPair(p);
    p.first = _nodes[7];
    p.second = 1;
    _nodes[6]->addNeighborPair(p);
    p.first = _nodes[0];
    p.second = 8;
    _nodes[7]->addNeighborPair(p);
    p.first = _nodes[1];
    p.second = 11;
    _nodes[7]->addNeighborPair(p);
    p.first = _nodes[6];
    p.second = 1;
    _nodes[7]->addNeighborPair(p);
    p.first = _nodes[8];
    p.second = 7;
    _nodes[7]->addNeighborPair(p);
    p.first = _nodes[7];
    p.second = 7;
    _nodes[8]->addNeighborPair(p);
    p.first = _nodes[6];
    p.second = 6;
    _nodes[8]->addNeighborPair(p);
    p.first = _nodes[2];
    p.second = 2;
    _nodes[8]->addNeighborPair(p);
    // Testcase for MST//////////////////////////////////////////
    // graph should be in _nodes////////////////////////////////
    vector<pair<Node *, pair<double, Node *>>> qheap; // <Node,key,predecessor>
    qheap.resize(_nodes.size());
    for (size_t i = 0; i < _nodes.size(); i++)
    {
        qheap[i].first = new Node();
        qheap[i].first->setFFinNode(_nodes[i]->getFFinNode());
        qheap[i].first->setNeighborsize(_nodes[i]->getNeighborsize());
        for (size_t j = 0; j < _nodes[i]->getNeighborsize(); j++)
        {
            qheap[i].first->setNeighborPair(j, _nodes[i]->getNeighborPair(j));
        }
        qheap[i].second.first = __DBL_MAX__;
        qheap[i].second.second = nullptr;
    }
    qheap[0].second.first = 0;
    // for (size_t i = 0; i < qheap.size(); i++)
    // {
    //     cout << "Node" << qheap[i].first->getFFinNode()->getName() << "has neighbor: ";
    //     for (size_t j = 0; j < qheap[i].first->getNeighborsize(); j++)
    //     {
    //         cout << qheap[i].first->getneighborNode(j)->getFFinNode()->getName() << " " << qheap[i].first->getneighborweight(j) << " ";
    //     }
    //     cout << endl;
    // }
}
void Placement::DecreaseKeyMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx, double key)
{
    return;
}
Node *Placement::extractMinMST(vector<pair<Node *, pair<double, Node *>>> &heap)
{
    Node *n;
    return n;
}