#include "Placement.h"

#include <limits>
#include <vector>
#include <limits>
#include <cstdlib>
#include <cmath>
using namespace std;
#define leafthresold 0.75 // TODO: can be changed
void Placement::mainLoop()
{
    // find same clk path-> calculate diamond -> construct graph according to cost -> find MST -> merge FFs
    // TODO:how to find same clk path , and loop it
    NodeList mst = findMST();
    int leafsize = 0;
    for (size_t i = 0; i < mst.size(); i++)
    {
        if (mst[i]->getNeighborsize() == 1)
        {
            mst[i]->setisleaf(true);
            leafsize++;
        }
        else
        {
            mst[i]->setisleaf(false);
        }
    }
    // TODO:test /////////////////////////////////////
    cout << "leafsize: " << leafsize << endl;
    for (size_t i = 0; i < _nodes.size(); i++)
    {
        free(_nodes[i]);
    }
    _nodes.clear();
    _nodes = mst;
    merge2FF(0, 1, 0);
    // test /////////////////////////////////////
    // print MST///////////////////////////////////////////////
    for (size_t i = 0; i < _nodes.size(); i++)
    {
        cout << "Node " << _nodes[i]->getFFinNode()->name() << " has neighbor: ";
        cout << " " << _nodes[i]->getNodeidxheap() << endl;
        map<string, pair<Node *, double>> neighbor = _nodes[i]->getneighbormap();
        for (const auto &pair : neighbor)
        {
            cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
        }
        cout << endl;
    }
    cout << _nodes.size() << endl;
    // print MST///////////////////////////////////////////////
    if (static_cast<double>(leafsize) / static_cast<double>(_nodes.size()) < leafthresold)
    { // do merge & store mst into _nodes
        for (size_t i = 0; i < _nodes.size(); i++)
        {

            free(_nodes[i]);
        }
        _nodes.clear();
        _nodes = mst;
        mergeFFinG();
    }
    else // greedy pick to merge
    {
        for (size_t i = 0; i < mst.size(); i++)
        {
            free(mst[i]);
        }
        mst.clear();
        unsigned target = rand() % _nodes.size();
        while (_nodes[target]->getNeighborsize() < 1)
        {
            target = rand() % _nodes.size();
        }
        map<string, pair<Node *, double>> neighbor = _nodes[target]->getneighbormap();
        string maxidx = neighbor.begin()->first;
        for (const auto &pair : neighbor)
        {
            if (pair.second.second > neighbor[maxidx].second)
            {
                maxidx = pair.first;
            }
        }
        // TODO: maybe randomly pick which FF to merge to ??
        merge2FF(target, neighbor[maxidx].first->getNodeidxheap(), 0);
    }
}

void Placement::mergeFFinG()
{
    while (_nodes.size() > 1)
    {
        // find a leaf
        unsigned idx = 0;
        while (_nodes[idx]->getisleaf() == false)
        {
            idx++;
        }
        auto mymap = *_nodes[idx]->getneighbormap().begin();
        unsigned idx2 = mymap.second.first->getNodeidxheap();
        merge2FF(idx, idx2, 0);
        break; // TODO: temp break
    }
    return;
}
void Placement::merge2FF(unsigned idx1, unsigned idx2, unsigned newffidx)
{
    // TODO: need to know which FF should be chosed
    // ex: merge two 1 bit, which 2 bit FF should be chosed?
    // erase to FF from graph(_nodes)/////////////////////////////////
    // map<string, pair<Node *, double>> neighbor = _nodes[idx1]->getneighbormap();
    // for (const auto &pair : neighbor)
    // {
    //     pair.second.first->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
    // }
    // neighbor = _nodes[idx2]->getneighbormap();
    // for (const auto &pair : neighbor)
    // {
    //     pair.second.first->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
    // }
    // _nodes[idx1]->clearNeighbor();
    // _nodes[idx2]->clearNeighbor();
    // erase to FF from graph(_nodes)/////////////////////////////////
    cout << _dataBase->getNumModules() << endl;
    for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    {
        cout << "module name" << _dataBase->module(i)->name() << endl;
    }

    return;
    Module *m1 = _nodes[idx1]->getFFinNode();
    Module *m2 = _nodes[idx2]->getFFinNode();
    // cal position///////////////////////////////////////////////
    double m1x, m1y, m2x, m2y;
    double newX, newY;
    double angle = 45 * M_PI / 180;
    m1x = m1->centerX() * cos(angle) - m1->centerY() * sin(angle);
    m1y = m1->centerX() * sin(angle) + m1->centerY() * cos(angle);
    m2x = m2->centerX() * cos(angle) - m2->centerY() * sin(angle);
    m2y = m2->centerX() * sin(angle) + m2->centerY() * cos(angle); // center of the square
    string m3name = _dataBase->module(_dataBase->getNumModules() - 1)->name();
    int num = stoi(m3name.substr(1));
    num += 1;
    m3name = "C" + to_string(num); // set name
    Module *m3 = new Module(m3name, _dataBase->ffLib(m1->cellType()->getnumBit(), newffidx), newX, newY);
    m3->clearPins();
    // Let m1's pins comes first
    for (size_t i = 0; i < m1->numInPins(); i++)
    {
        m3->addPin(m1->InPin(i));
    }
    for (size_t i = 0; i < m2->numInPins(); i++)
    {
        m3->addPin(m2->InPin(i));
        num = stoi(m3->pin(m2->totnumPins() - 1)->name().substr(1));
        num += 1;
        m3name = "D" + to_string(num); // set name
        m3->pin(m2->totnumPins() - 1)->setPinName(m3name);
    }
    for (size_t i = 0; i < m1->numOutPins(); i++)
    {
        m3->addPin(m1->OutPin(i));
    }
    for (size_t i = 0; i < m2->numOutPins(); i++)
    {
        m3->addPin(m2->OutPin(i));
        num = stoi(m3->pin(m2->totnumPins() - 1)->name().substr(1));
        num += 1;
        m3name = "Q" + to_string(num); // set name
        m3->pin(m2->totnumPins() - 1)->setPinName(m3name);
    }
    m3->addPin(m1->pin(m1->totnumPins() - 1));
    // TODO: this is erase extra clk pin , but May need to add index to lower the runtime)
    for (size_t i = 0; i < m1->pin(m1->totnumPins() - 1)->net()->numPins(); i++)
    {
        if (m1->pin(m1->totnumPins() - 1)->net()->pin(i) == m2->pin(m2->totnumPins() - 1))
        {
            Pin *p = m1->pin(m1->totnumPins() - 1)->net()->pin(i);
            m1->pin(m1->totnumPins() - 1)->net()->erasePin(i);
            free(p);
            break;
        }
    }
    // update Module in Pin
    for (size_t i = 0; i < m3->totnumPins(); i++)
    {
        m3->pin(i)->setModulePtr(m3);
    }
    // update x,y offset
    for (size_t i = 0; i < m3->totnumPins(); i++)
    {
        m3->pin(i)->setOffset(m3->cellType()->pinOffsetX(i), m3->cellType()->pinOffsetY(i));
        m3->pin(i)->setPosition(m3->x() + m3->pin(i)->xOffset(), m3->y() + m3->pin(i)->yOffset());
    }
    //  so D0~Dbits/2 is m1's old pin , and we keep m1's CLK pin
    // TODO: index????????????????????????????????????????????????????????????????????????????????????????
    // TODO: update History
    //  clear FFs with no neighbor out of graph(_nodes)/////////////////////////////////
    for (auto it = _nodes.begin(); it != _nodes.end();)
    {
        if ((*it)->getNeighborsize() == 0)
        {
            delete *it;
            it = _nodes.erase(it);
        }
        else
        {
            (*it)->setNodeidxheap(it - _nodes.begin());
            ++it;
        }
    }
    return;
}

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

NodeList Placement::findMST()
{
    // Testcase for MST//////////////////////////////////////////
    setNodesize(9);
    pair<Node *, double> p;
    vector<Module *> j;
    j.clear();
    string name = "a";
    for (size_t i = 0; i < 9; i++)
    {
        Module *ff = new Module();
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
        _nodes[i]->setNodeidxheap(i);
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
        qheap[i].first = _nodes[i];
        qheap[i].second.first = __DBL_MAX__;
        qheap[i].second.second = nullptr;
    }
    qheap[0].second.first = 0;
    vector<pair<Node *, pair<double, Node *>>> mstHeap;
    NodeList mst;
    while (qheap.size() > 0)
    {
        pair<Node *, pair<double, Node *>> min = extractMinMST(qheap);
        min.first->setNodeidxheap(-1);
        // print MST///////////////////////////////////////////////
        cout << "Node: " << min.first->getFFinNode()->name() << "   key : " << min.second.first;
        if (min.second.second != nullptr)
        {
            cout << "predecessor : " << min.second.second->getFFinNode()->name();
        }
        cout << endl;
        // print MST///////////////////////////////////////////////
        map<string, pair<Node *, double>> neighbor = min.first->getneighbormap(); // previous node's neighbor
        for (const auto &pair : neighbor)
        {
            if (pair.second.first->getNodeidxheap() != -1)
            {
                if (pair.second.second < qheap[pair.second.first->getNodeidxheap()].second.first)
                {
                    qheap[pair.second.first->getNodeidxheap()].second.second = min.first; // set predecessor
                    DecreaseKeyMST(qheap, pair.second.first->getNodeidxheap(), pair.second.second);
                }
            }
        }
        mstHeap.push_back(min);
    }
    for (size_t i = 0; i < mstHeap.size(); ++i)
    {
        mst.push_back(new Node(mstHeap[i].first->getFFinNode()));
        mst[i]->setNodeidxheap(i);
        mstHeap[i].first->setNodeidxheap(i);
        mst[i]->setisleaf(false);
    }
    for (size_t i = mstHeap.size() - 1; i >= 1; --i)
    {
        mst[i]->addNeighborPair(make_pair(mst[mstHeap[i].second.second->getNodeidxheap()], mstHeap[i].second.first));
        unsigned id = mstHeap[i].second.second->getNodeidxheap();
        mst[id]->addNeighborPair(make_pair(mst[i], mstHeap[i].second.first));
    }
    // print MST///////////////////////////////////////////////
    for (size_t i = 0; i < mst.size(); i++)
    {
        cout << "Node " << mst[i]->getFFinNode()->name() << " has neighbor: ";
        cout << " " << mst[i]->getNodeidxheap() << endl;
        map<string, pair<Node *, double>> neighbor = mst[i]->getneighbormap();
        for (const auto &pair : neighbor)
        {
            cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
        }
        cout << endl;
    }
    // print MST///////////////////////////////////////////////
    mstHeap.clear();
    qheap.clear();
    return mst;
    //  print the heap//////////////////////////////////////////
    // for (size_t i = 0; i < qheap.size(); i++)
    // {
    //     cout << "Node " << qheap[i].first->getFFinNode()->name() << " has neighbor: ";
    //     cout << " " << qheap[i].first->getNodeidxheap() << " " << endl;
    //     cout << "key : " << qheap[i].second.first << endl;
    //     map<string, pair<Node *, double>> neighbor = qheap[i].first->getneighbormap();
    //     for (const auto &pair : neighbor)
    //     {
    //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //     }
    //     cout << endl;
    // }
    // print the heap//////////////////////////////////////////
}

void Placement::DecreaseKeyMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx, double key)
{ // idx is the index of the node to be decreased in the heap
    unsigned targetidx = idx;
    heap[idx].second.first = key;
    while (targetidx > 0 && heap[(targetidx - 1) / 2].second.first > key)
    {
        swapNodeMST(heap, targetidx, (targetidx - 1) / 2);
        targetidx = (idx - 1) / 2;
    }
    return;
}

pair<Node *, pair<double, Node *>> Placement::extractMinMST(vector<pair<Node *, pair<double, Node *>>> &heap)
{
    pair<Node *, pair<double, Node *>> n = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap[0].first->setNodeidxheap(0);
    heap.pop_back();
    unsigned targetidx = 0;
    while (1)
    {
        if (targetidx * 2 + 1 >= heap.size())
        {
            break;
        }
        double key1 = __DBL_MAX__;
        double key2 = __DBL_MAX__;
        if (targetidx * 2 + 1 < heap.size())
        {
            key1 = heap[targetidx * 2 + 1].second.first;
        }
        if (targetidx * 2 + 2 < heap.size())
        {
            key2 = heap[targetidx * 2 + 2].second.first;
        }
        if (heap[targetidx].second.first > key1 || heap[targetidx].second.first > key2)
        {
            if (key1 < key2)
            {
                swapNodeMST(heap, targetidx, targetidx * 2 + 1);
                targetidx = targetidx * 2 + 1;
            }
            else if (key1 > key2)
            {
                swapNodeMST(heap, targetidx, targetidx * 2 + 2);
                targetidx = targetidx * 2 + 2;
            }
        }
        else
        {
            break;
        }
    }

    return n;
}

void Placement::swapNodeMST(vector<pair<Node *, pair<double, Node *>>> &heap, unsigned idx1, unsigned idx2)
{
    pair<Node *, pair<double, Node *>> temp;
    heap[idx1].first->setNodeidxheap(idx2);
    heap[idx2].first->setNodeidxheap(idx1);
    temp = heap[idx1];
    heap[idx1] = heap[idx2];
    heap[idx2] = temp;
    return;
}