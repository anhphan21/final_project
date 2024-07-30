#include "Placement.h"
#include <mutex>
#include <thread>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <limits.h>
#include <vector>
#include <random>
#include <set>
#include <math.h>
#include <set>
#include <cfloat>
#include <queue>
using namespace std;

#define leafthresold 0.75 // TODO: can be changed
#define __DBL_MAX__         1.7976931348623158e+308 /* max value */
struct Edges
{
    Module *ff;
    double y;
    double startx;
    double endx;
    bool isIN;
};
typedef struct Edges Edge;
void Placement::mainLoop()
{
    // find same clk path-> calculate diamond -> construct graph according to cost -> find MST -> merge FFs
    // TODO:how to find same clk path , and loop it
    NodeList mst = findMST();
    // print MST ///////////////////////////////////////////////
    for (size_t i = 0; i < mst.size(); i++)
    {
        cout << "Node " << mst[i]->getFFinNode()->name() << " has neighbor: ";
        map<string, pair<Node *, double>> neighbor = mst[i]->getneighbormap();
        for (const auto &pair : neighbor)
        {
            cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
        }
        cout << endl;
    }
    // print MST ///////////////////////////////////////////////
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
    if (static_cast<double>(leafsize) / static_cast<double>(_nodes.size()) < leafthresold) // leaf not too much
    {                                                                                      // do merge & store mst into _nodes
        cout << "do merge on mst\n";
        for (size_t i = 0; i < _nodes.size(); i++)
        {
            free(_nodes[i]);
        }
        _nodes.clear();
        _nodes = mst;
        mergeFFinG();
    }
    else // greedy pick to merge leaf too much
    {
        cout << "leaf too much, merge randomly\n";
        for (size_t i = 0; i < mst.size(); i++)
        {
            free(mst[i]);
        }
        mst.clear();
        cout << "node size : " << _nodes.size() << endl;
        while (_nodes.size() > 1)
        {
            random_device rd;
            mt19937 generator(rd());
            uniform_int_distribution<unsigned> distribution(0, _nodes.size() - 1);
            unsigned target = distribution(generator);
            cout << "target : " << target << endl;
            while (_nodes[target]->getNeighborsize() < 1)
            {
                target = distribution(generator);
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
        if (_nodes[idx]->getneighborweight(_nodes[idx2]->getFFinNode()->name()) < 0)
        {
            merge2FF(idx, idx2, 0);
        }
        else
        {
            cout << "won't improve\n";
            eraseEdge(idx, idx2);
        }
    }
    return;
}
void Placement::eraseEdge(unsigned idx1, unsigned idx2)
{
    _nodes[idx1]->eraseNeighbor(_nodes[idx2]->getFFinNode()->name());
    _nodes[idx2]->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
    // delete nodes with no neighbor
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
void Placement::merge2FF(unsigned idx1, unsigned idx2, unsigned newffidx)
{
    // TODO: need to know which FF should be chosed
    // ex: merge two 1 bit, which 2 bit FF should be chosed?
    // erase to FF from graph(_nodes)/////////////////////////////////
    map<string, pair<Node *, double>> neighbor = _nodes[idx1]->getneighbormap();
    for (const auto &pair : neighbor)
    {
        pair.second.first->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
    }
    neighbor = _nodes[idx2]->getneighbormap();
    for (const auto &pair : neighbor)
    {
        pair.second.first->eraseNeighbor(_nodes[idx2]->getFFinNode()->name());
    }
    _nodes[idx1]->clearNeighbor();
    _nodes[idx2]->clearNeighbor();
    // erase to FF from graph(_nodes)/////////////////////////////////
    Module *m1 = _nodes[idx1]->getFFinNode();
    Module *m2 = _nodes[idx2]->getFFinNode();
    // cal position///////////////////////////////////////////////
    double m1x, m1y, m2x, m2y;
    double newX, newY;
    m1x = m1->centerX();
    m1y = m1->centerY();
    m2x = m2->centerX();
    m2y = m2->centerY();
    // m1->setRadius(3000);
    // m2->setRadius(3000);
    Rhombus *r1 = new Rhombus(m1x, m1y, m1->radius());
    Rhombus *r2 = new Rhombus(m2x, m2y, m2->radius());
    pair<double, double> newloc = r1->findCentroidIntersect(*r1, *r2);
    newX = newloc.first;
    newY = newloc.second;
    // cal position///////////////////////////////////////////////
    string m3name = _dataBase->module(_dataBase->getNumModules() - 1)->name();
    string letters;
    string numbers;
    for (char c : m3name)
    {
        if (std::isdigit(c))
        {
            numbers += c;
        }
        else
        {
            letters += c;
        }
    }
    int num = stoi(numbers);
    m3name = letters + to_string(num + 1);
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<unsigned> distribution(0, _dataBase->getNumfflibBit((m1->cellType()->getnumBit()) * 2) - 1);
    newffidx = distribution(generator);
    Module *m3 = new Module(m3name, _dataBase->ffLib((m1->cellType()->getnumBit()) * 2, newffidx), newX, newY);
    m3->clearPins();
    m3->setPinsize(m3->cellType()->getnumBit() * 2 + 1);
    int pinid = 0;
    for (size_t i = 0; i < m1->numInPins(); i++)
    {
        if (m1->InPin(i)->name() != "CLK" && m1->OutPin(i)->name() != "clk")
        {
            string str = "D";
            str = str + to_string(pinid);
            m1->InPin(i)->setPinName(str);
            pinid++;
        }
    }
    for (size_t i = 0; i < m2->numInPins(); i++)
    {
        if (m2->InPin(i)->name() != "CLK" && m2->OutPin(i)->name() != "clk")
        {
            string str = "D";
            str = str + to_string(pinid);
            m2->InPin(i)->setPinName(str);
            pinid++;
        }
    }
    pinid = 0;
    for (size_t i = 0; i < m1->numOutPins(); i++)
    {
        if (m1->OutPin(i)->name() != "CLK" && m1->OutPin(i)->name() != "clk")
        {
            string str = "Q";
            str = str + to_string(pinid);
            m1->OutPin(i)->setPinName(str);
            pinid++;
        }
    }
    for (size_t i = 0; i < m2->numOutPins(); i++)
    {
        if (m2->OutPin(i)->name() != "CLK" && m2->OutPin(i)->name() != "clk")
        {
            string str = "Q";
            str = str + to_string(pinid);
            m2->OutPin(i)->setPinName(str);
            pinid++;
        }
    }
    for (size_t i = 0; i < m3->cellType()->getnumBit() / 2; i++)
    {
        m3->setInPin(i, m1->InPin(i));
        m3->InPin(i)->setModulePtr(m3);
    }
    for (size_t i = m3->cellType()->getnumBit() / 2; i < m3->cellType()->getnumBit(); i++)
    {
        m3->setInPin(i, m2->InPin(i - m3->cellType()->getnumBit() / 2));
        m3->InPin(i)->setModulePtr(m3);
    }
    for (size_t i = 0; i < m3->cellType()->getnumBit() / 2; i++)
    {
        m3->setOutPin(i, m1->OutPin(i));
        m3->OutPin(i)->setModulePtr(m3);
    }
    for (size_t i = m3->cellType()->getnumBit() / 2; i < m3->cellType()->getnumBit(); i++)
    {
        m3->setOutPin(i, m2->OutPin(i - m3->cellType()->getnumBit() / 2));
        m3->OutPin(i)->setModulePtr(m3);
    }
    m3->setInPin(m3->cellType()->clkPinIdx(), m1->InPin(m1->cellType()->clkPinIdx()));
    m3->InPin(m3->cellType()->clkPinIdx())->setModulePtr(m3);
    // update Module in Pin
    // update x,y offset
    // TODO: may be wrong here
    for (size_t i = 0; i < m3->totnumPins(); i++)
    {
        m3->pin(i)->setOffset(m3->cellType()->pinOffsetX(i), m3->cellType()->pinOffsetY(i));
        m3->pin(i)->setPosition(m3->x() + m3->pin(i)->xOffset(), m3->y() + m3->pin(i)->yOffset());
    }
    //  so D0~Dbits/2 is m1's old pin , and we keep m1's CLK pin
    for (size_t i = 0; i < m2->InPin(m2->cellType()->clkPinIdx())->net()->numPins(); i++)
    {
        if (m2->InPin(m2->cellType()->clkPinIdx())->net()->pin(i) == m2->InPin(m2->cellType()->clkPinIdx()))
        {
            m2->InPin(m2->cellType()->clkPinIdx())->net()->erasePin(i);
            break;
        }
    }
    for (size_t i = 0; i < _dataBase->getNumPins(); i++)
    {
        if (m2->InPin(m2->cellType()->clkPinIdx()) == _dataBase->pin(i))
        {
            _dataBase->erasePin(i);
            break;
        }
    }
    Pin *p = m2->InPin(m2->cellType()->clkPinIdx());
    free(p->history());
    free(p);
    //  clear FFs with no neighbor out of graph(_nodes)/////////////////////////////////
    // free m1 and m2
    for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    {
        if (_dataBase->module(i) == m1)
        {
            _dataBase->eraseModule(i);
            break;
        }
    }
    for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    {
        if (_dataBase->module(i) == m2)
        {
            _dataBase->eraseModule(i);
            break;
        }
    }
    for (size_t i = 0; i < _dataBase->getNumFF(); i++)
    {
        if (_dataBase->ff(i) == m1)
        {
            _dataBase->eraseFF(i);
            break;
        }
    }
    for (size_t i = 0; i < _dataBase->getNumFF(); i++)
    {
        if (_dataBase->ff(i) == m2)
        {
            _dataBase->eraseFF(i);
            break;
        }
    }

    free(m1);
    free(m2);
    _dataBase->addModule(m3);
    _dataBase->addFF(m3);
    m3->setCenterPosition(newX, newY);
    // check boundary
    if (m3->x() < _dataBase->getBoundaryLeft())
    {
        m3->setPosition(_dataBase->getBoundaryLeft(), m3->y());
    }
    if (m3->y() < _dataBase->getBoundaryBottom())
    {
        m3->setPosition(m3->x(), _dataBase->getBoundaryBottom());
    }
    if (m3->x() + m3->width() > _dataBase->getBoundaryRight())
    {
        m3->setPosition(_dataBase->getBoundaryRight() - m3->width(), m3->y());
    }
    if (m3->y() + m3->height() > _dataBase->getBoundaryTop())
    {
        m3->setPosition(m3->x(), _dataBase->getBoundaryTop() - m3->height());
    }

    // delete nodes with no neighbor
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
    // graph should be in _nodes////////////////////////////////
    vector<pair<Node *, pair<double, Node *>>> qheap; // <Node,key,predecessor>
    qheap.resize(_nodes.size());
    for (size_t i = 0; i < _nodes.size(); i++)
    {
        _nodes[i]->setNodeidxheap(i);
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
    // print MST ///////////////////////////////////////////////
    // for (size_t i = 0; i < mst.size(); i++)
    // {
    //     cout << "Node " << mst[i]->getFFinNode()->name() << " has neighbor: ";
    //     map<string, pair<Node *, double>> neighbor = mst[i]->getneighbormap();
    //     for (const auto &pair : neighbor)
    //     {
    //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //     }
    //     cout << endl;
    // }
    // print MST ///////////////////////////////////////////////
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
double cal_distance(pair<double, double> A, pair<double, double> B)
{
    return (abs(A.first - B.first) + abs(A.second - B.second));
}

bool overlap_ornot(vector<Rhombus> &input_rhombus, double &leftBound, double &rightBound, double &botBound, double &topBound)
{
    if (input_rhombus.empty())
        return false;
    leftBound = numeric_limits<double>::lowest();
    rightBound = numeric_limits<double>::max();
    botBound = numeric_limits<double>::lowest();
    topBound = numeric_limits<double>::max();
    // Rotate 45 deg
    for (auto &rhombus : input_rhombus)
    {
        rhombus.RotatePeak(45);
    }
    for (Rhombus &rhombus : input_rhombus)
    {
        leftBound = max(leftBound, rhombus.peak(3).x);
        rightBound = min(rightBound, rhombus.peak(1).x);
        botBound = max(botBound, rhombus.peak(3).y);
        topBound = min(topBound, rhombus.peak(1).y);
    }

    for (auto &rhombus : input_rhombus)
    {
        rhombus.RotatePeak(-45);
    }
    // Check if vec. rhombus are overlap or not ?
    if ((leftBound > rightBound) || (botBound > topBound))
    {
        return false;
    }
    else
    {
        return true;
    }
}
void Placement::constructGraph()
{
    int num_FF = _dataBase->getNumFF();
    int max_BitFF = _dataBase->getMaxBitFFLib(); // the max bit FF in FFlib

    _nodes.clear();
    for (int i = 0; i < num_FF; ++i)
    {
        Node *n = new Node;
        n->setFFinNode(_dataBase->ff(i));
        _name2Node[n->getFFinNode()->name()] = n;
        _nodes.push_back(n);
    }
    for (int i = 0; i < _dataBase->getNumClkNets(); ++i)
    {
        for (int j = 0; j < _dataBase->getClkNets()[i]->numPins(); ++j)
        {
            Module *currentFF = _dataBase->getClkNets()[i]->pin(j)->module();
            if (currentFF == nullptr || currentFF->isFF() == 0)
                continue;
            if (currentFF->cellType()->numBit() >= max_BitFF)
                continue;
            for (int k = j + 1; k < _dataBase->getClkNets()[i]->numPins(); ++k)
            {
                Module *compareFF = _dataBase->getClkNets()[i]->pin(k)->module();
                if (compareFF == nullptr || compareFF->isFF() == 0)
                    continue;
                if (compareFF->cellType()->numBit() >= max_BitFF || compareFF->cellType()->numBit() != currentFF->cellType()->numBit())
                    continue;
                // should modify here (preGate and nextGate)
                Rhombus r_current(_dataBase->getClkNets()[i]->pin(j)->net()->OutputPin()->x(), _dataBase->getClkNets()[i]->pin(j)->net()->OutputPin()->y(), currentFF->radius());
                Rhombus r_compare(_dataBase->getClkNets()[i]->pin(k)->net()->OutputPin()->x(), _dataBase->getClkNets()[i]->pin(k)->net()->OutputPin()->y(), compareFF->radius());
                //
                // if (overlap_ornot(r_current, r_compare) == 1)
                // {
                //     _name2Node[currentFF->name()]->addNeighborPair({_name2Node[compareFF->name()], cal_cost(currentFF, compareFF)});
                //     _name2Node[compareFF->name()]->addNeighborPair({_name2Node[currentFF->name()], cal_cost(compareFF, currentFF)});
                // }
            }

            // if (_dataBase->getClkNets()[i]->pin(j)->module() != nullptr && _dataBase->getClkNets()[i]->pin(j)->module()->isFF() && _dataBase->getClkNets()[i]->pin(j)->module()->cellType()->numBit() < max_BitFF)
            // {
            //     Module *module_i_j = _dataBase->getClkNets()[i]->pin(j)->module();
            //     for (int k = j + 1; k < _dataBase->getClkNets()[i]->numPins(); ++k)
            //     {
            //         Module *module_i_k = _dataBase->getClkNets()[i]->pin(k)->module();
            //         Rhombus r_i_j(_dataBase->getClkNets()[i]->pin(j)->net()->OutputPin()->x(), _dataBase->getClkNets()[i]->pin(j)->net()->OutputPin()->y(), module_i_j->radius());
            //         Rhombus r_i_k(_dataBase->getClkNets()[i]->pin(k)->net()->OutputPin()->x(), _dataBase->getClkNets()[i]->pin(k)->net()->OutputPin()->y(), module_i_k->radius());

            //         if (module_i_k == nullptr || module_i_k->isFF() == 0 || module_i_k->cellType()->numBit() >= max_BitFF)
            //             continue;
            //         else if (overlap_ornot(r_i_j, r_i_k) && module_i_j->cellType()->numBit() == module_i_k->cellType()->numBit()) // they are overlapping ,in the same clknet,have the same bit FF and both of them are not larger than max_BitFF
            //         {
            //             cout << "name: " << module_i_j->name() << "、" << module_i_k->name() << " ,cost: " << cal_cost(module_i_j, module_i_k) << endl;
            //             cout << module_i_j->InPin(0)->slack() << " " << module_i_k->InPin(0)->slack() << endl;
            //             _name2Node[module_i_j->name()]->addNeighborPair({_name2Node[module_i_k->name()], cal_cost(module_i_j, module_i_k)});
            //             _name2Node[module_i_k->name()]->addNeighborPair({_name2Node[module_i_j->name()], cal_cost(module_i_k, module_i_j)});
            //         }
            //     }
            // }
        }
    }
}


double Placement::cal_cost(Module *ffN, Module *ff0) // ffN is primary
{
    // // initail cost
    // double initial_cost = 0;
    // double initial_TNS_cost = 0;
    // if (ffN->InPin(0)->slack() < 0)
    //     initial_TNS_cost += ffN->InPin(0)->slack();
    // if (ff0->InPin(0)->slack() < 0)
    //     initial_TNS_cost += ff0->InPin(0)->slack();
    // double initial_Power_cost = 0;
    // initial_Power_cost = _dataBase->getBeta() * ffN->getPower() + _dataBase->getBeta() * ff0->getPower();
    // double initial_Area_cost = 0;
    // initial_Area_cost = _dataBase->getGamma() * ffN->area() + _dataBase->getGamma() * ff0->area();

    // //
    // double TNS_cost = 0;

    // for (int i = 0; i < ffN->numInPins() - 1 /*the last Inpin is CLK*/; ++i) // ffN and ff0 numInPins must be equal
    // {
    //     double old_slack1 = ffN->InPin(i)->slack();
    //     double old_slack2 = ff0->InPin(i)->slack();
    //     Rhombus r1(ffN->InPin(i)->net()->OutputPin()->x(), ffN->InPin(i)->net()->OutputPin()->y(), ffN->radius());
    //     Rhombus r2(ff0->InPin(i)->net()->OutputPin()->x(), ffN->InPin(i)->net()->OutputPin()->y(), ff0->radius());
    //     pair<double, double> new_location = Rhombus::findCentroidIntersect(r1, r2);
    //     ffN->setPosition(new_location.first, new_location.second);
    //     ff0->setPosition(new_location.first, new_location.second);
    //     // cout<<ffN->InPin(i)->slack()
    //     _dataBase->updateSlack(ffN->InPin(i));
    //     _dataBase->updateSlack(ff0->InPin(i));
    //     if (ffN->InPin(i)->slack() < 0)
    //         TNS_cost += ffN->InPin(i)->slack();
    //     if (ff0->InPin(i)->slack() < 0)
    //         TNS_cost += ff0->InPin(i)->slack();
    //     // turn back
    //     ffN->setPosition(ffN->InPin(i)->oldX(), ffN->InPin(i)->oldY());
    //     ff0->setPosition(ff0->InPin(i)->oldX(), ff0->InPin(i)->oldY());
    //     ffN->InPin(i)->setSlack(old_slack1);
    //     ff0->InPin(i)->setSlack(old_slack2);
    // }
    // _dataBase->totalCost(1);
    // _dataBase->unMarkedDPin();
    // int next_level_FF = 0;
    // next_level_FF = pow(2, log2(ffN->cellType()->numBit()) + 1);
    // double Power_cost = 0;
    // Power_cost = _dataBase->getBeta() * _dataBase->getFFlib(next_level_FF)->getPower();
    // double Area_cost = 0;
    // Area_cost = _dataBase->getGamma() * _dataBase->getFFlib(next_level_FF)->getArea();

    // initial_cost = initial_TNS_cost + initial_Power_cost + initial_Area_cost;
    // double new_cost = TNS_cost + Power_cost + Area_cost;
    // // cout << ffN->name() << " " << ff0->name() << " ,cost: " << initial_cost << " " << new_cost << endl;
    // return (new_cost - initial_cost);
}

Rhombus Placement::findInputRegion(Module *ff)
{
    // ff must be 1 bit FF
    if (ff->InPin(0)->net()->OutputPin()->module() == nullptr) // directory to input pin
    {
        Rhombus ans(ff->InPin(0)->net()->OutputPin()->x(), ff->InPin(0)->net()->OutputPin()->y(), 0);
        return ans;
    }
    else
    {
        double ff_original_slack = ff->InPin(0)->getSlackInfor()->slack();
        double dis_delay = _dataBase->getDisplacementDelay();
        double WL_D_N = abs(ff->InPin(0)->net()->OutputPin()->x() - ff->InPin(0)->x()) + abs(ff->InPin(0)->net()->OutputPin()->y() - ff->InPin(0)->y());
        double radius = (ff_original_slack + dis_delay * WL_D_N) / dis_delay;
        Rhombus ans(ff->InPin(0)->net()->OutputPin()->x(), ff->InPin(0)->net()->OutputPin()->y(), radius);
        return ans;
    }
}

vector<Rhombus> Placement::findOutputRegion(Module *ff)
{
    // ff must be 1 bit FF
    // vector<Rhombus> multi_region;
    // for (int i = 0; i < ff->get_outputFF().size(); ++i)
    // {
    //     double slack = ff->get_outputFF()[i].second->InPin(0)->slack();
    //     double dis_delay = _dataBase->getDisplacementDelay();
    //     double WL_Q_0;
    //     if (ff->get_outputFF()[i].first == nullptr)
    //     {
    //         WL_Q_0 = abs(ff->OutPin(0)->x() - ff->get_outputFF()[i].second->InPin(0)->x()) + abs(ff->OutPin(0)->y() - ff->get_outputFF()[i].second->InPin(0)->y());
    //         double radius = (slack + dis_delay * WL_Q_0) / dis_delay;
    //         Rhombus ans(ff->get_outputFF()[i].second->InPin(0)->x(), ff->get_outputFF()[i].second->InPin(0)->y(), radius);
    //         multi_region.push_back(ans);
    //     }
    //     else // curr_FF 和 out_FF 之間有gate
    //     {
    //         WL_Q_0 = abs(ff->OutPin(0)->x() - ff->get_outputFF()[i].first->InPin(0)->x()) + abs(ff->OutPin(0)->y() - ff->get_outputFF()[i].first->InPin(0)->y());
    //         double radius = (slack + dis_delay * WL_Q_0) / dis_delay;
    //         Rhombus ans(ff->get_outputFF()[i].first->InPin(0)->x(), ff->get_outputFF()[i].first->InPin(0)->y(), radius);
    //         multi_region.push_back(ans);
    //     }
    // }
    // return multi_region;
}
void Placement::constructFeasible(Module *ff, Rhombus in, vector<Rhombus> out)
{
    if (out.empty())
        return;
    out.push_back(in);              //in_and_out
    double Feas_x1=0, Feas_y1=0, Feas_x2=0, Feas_y2=0;
    if (overlap_ornot(out, Feas_x1, Feas_x2, Feas_y1, Feas_y2) == 1)
    { // if return is true then Feas_x1, Feas_x2, Feas_y1, Feas_y2 have correct value
        ff->getFeasibleRegion()->setInf(Feas_x1, Feas_y1, Feas_x2, Feas_y2); // feasibleRegion still be retangleable
    }
    else 
    {
        cout<<"feasibleRegion not found\n"<<endl;
    }
}
void Placement::netListGraph() {
    int numThreads = thread::hardware_concurrency(); // 根據硬體條件選擇線程數量
    numThreads = 1;
    int numFF = this->_dataBase->getNumFF();
    int chunkSize = (numFF + numThreads - 1) / numThreads; // 計算每個線程處理的範圍
    mutex mtx; // 保護輸出和共享資源

    auto processFFRange = [&](int start, int end) 
    {
        for (int i = start; i < end; ++i) {
            deque<pair<Pin*, vector<int>>> que;
            for (int j = 0; j < this->_dataBase->ff(i)->numOutPins(); j++) {
                vector<int> a;
                a.push_back(this->_dataBase->ff(i)->No);
                que.push_back({ this->_dataBase->ff(i)->OutPin(j), a });
            }
            while (!que.empty()) {
                Module* moduleptr = que.front().first->module();
                if (moduleptr->isFF() && moduleptr->name() != this->_dataBase->ff(i)->name()) {
                    // que.front().second.size()-1 is moduleptr BenZen
                    // que.front().second.size()-2 is prelevel Fatboy of moduleptr 
                    Module * PreModule = this->getDatabase()->getIntModule(que.front().second[que.front().second.size()-2]);
                    if (PreModule->isFF())
                    {
                        this->_dataBase->ff(i)->_outputFF.insert({nullptr,moduleptr});
                    }
                    if (!PreModule->isFF())
                    {
                        this->_dataBase->ff(i)->_outputFF.insert({ PreModule,moduleptr });
                    }
                    que.front().second.clear();
                    que.front().second.shrink_to_fit();
                    que.pop_front();
                    continue;
                }
                if (moduleptr == nullptr) {
                    lock_guard<mutex> lock(mtx);
                    cout << "BUG" << endl;
                    exit(0);
                }
                for (int j = 0; j < moduleptr->numOutPins(); j++) {
                    for (int z = 0; z < moduleptr->OutPin(j)->net()->numPins(); z++) {
                        bool IO_Design = 0;
                        if (moduleptr->OutPin(j)->net()->pin(z)->module() == nullptr) {
                            if (this->_dataBase->IODesign.find(moduleptr->OutPin(j)->net()->pin(z)->name()) != this->_dataBase->IODesign.end()) {
                                continue;
                            }
                            else {
                                exit(0);
                            }
                        }
                        auto it = std::find(que.front().second.begin(), que.front().second.end(), moduleptr->OutPin(j)->net()->pin(z)->module()->No);
                        if (z == moduleptr->OutPin(j)->net()->getOutIdx() || it != que.front().second.end()) {
                            if (it != que.front().second.end() && moduleptr->OutPin(j)->net()->pin(z)->module()->name() == "C101355") {

                            }
                        }
                        else {
                            que.push_back({ moduleptr->OutPin(j)->net()->pin(z), que.front().second });
                            que.back().second.push_back(moduleptr->OutPin(j)->net()->pin(z)->module()->No);
                             }
                    }
                }
                /*cout << "Master: " << que.front().first->module()->name() << endl;
                for (auto a : que.front().second)
                {
                    cout <<" "<< this->getDatabase()->getIntModule(a)->name() << endl;
                }
                cout << endl;*/
                que.front().second.clear();
                que.front().second.shrink_to_fit();
                que.pop_front();
            }
            if (i % 1000 == 0) 
            {
                cout << i << endl;
                cout << this->getDatabase()->ff(i)->name() << endl;
                for (auto it = this->getDatabase()->ff(i)->_outputFF.begin(); it != this->getDatabase()->ff(i)->_outputFF.end(); it++)
                {
                    if (it->first == nullptr)
                    {
                        cout << "Gate: nullptr   FF:" << it->second->name() << endl;
                    }
                    else
                    {
                        cout << "Gate: " << it->first->name() << "   FF:" << it->second->name() << endl;
                    }
                }
            }

        } //Inside is gragh Funtion
    };

    vector<thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        int start = t * chunkSize;
        int end = min(start + chunkSize, numFF);
        threads.emplace_back(processFFRange, start, end);
    }

    for (auto& t : threads) {
        t.join();
    }
}


void Placement::netListGraph(Module* moduleOrigin)
{
    
        deque< pair<Pin*, vector<int>>> que;
        for (int j = 0; j < moduleOrigin->numOutPins(); j++)
        {
            vector<int> a;
            a.push_back(moduleOrigin->No);
            que.push_back({ moduleOrigin->OutPin(j),a });
        }
        while (!que.empty())
        {
            Module* moduleptr = que.front().first->module();
            if (moduleptr->isFF() && moduleptr->name() != moduleOrigin->name())
            {
                Module* PreModule = this->getDatabase()->getIntModule(que.front().second[que.front().second.size() - 2]);
                if (PreModule->isFF())
                {
                    moduleOrigin->_outputFF.insert({ nullptr,moduleptr });
                }
                if (!PreModule->isFF())
                {
                    moduleOrigin->_outputFF.insert({ PreModule,moduleptr });
                }
                que.front().second.clear();
                que.front().second.shrink_to_fit();
                que.pop_front();
                continue;
            }
            if (moduleptr == nullptr)
            {
                cout << "BUG" << endl;
                exit(0);
            }
            for (int j = 0; j < moduleptr->numOutPins(); j++)
            {
                for (int z = 0; z < moduleptr->OutPin(j)->net()->numPins(); z++)
                {

                    bool IO_Design = 0;
                    if (moduleptr->OutPin(j)->net()->pin(z)->module() == nullptr) //如果今天pin沒任何的module 就不要用
                    {
                        if (this->_dataBase->IODesign.find(moduleptr->OutPin(j)->net()->pin(z)->name()) != this->_dataBase->IODesign.end()) //pin 沒有module可能是IO Design
                        {/*
                            cout << "I'm IODesign: " << moduleptr->OutPin(j)->net()->pin(z)->name() << endl;*/
                            continue; //如果是IO Desgin 就不要繼續traverse
                        }
                        else
                        {
                            exit(0);
                        }
                    }
                    auto it = std::find(que.front().second.begin(), que.front().second.end(), moduleptr->OutPin(j)->net()->pin(z)->module()->No);
                    if (z == moduleptr->OutPin(j)->net()->getOutIdx() || it != que.front().second.end()) //為輸入端 抑或是有 Latch
                    {
                        if (it != que.front().second.end() && moduleptr->OutPin(j)->net()->pin(z)->module()->name() == "C101355")
                        {
                            /*cout << endl << endl;
                            cout << "---------------------" << endl;
                            cout << "Latch" << endl;*/
                        }
                    }
                    else
                    {
                        /* cout << "HE" << endl;*/
                         /*cout << "Drop IN " << moduleptr->OutPin(j)->net()->pin(z)->module()->name()<< endl;*/
                        que.push_back({ moduleptr->OutPin(j)->net()->pin(z), que.front().second });
                        que.back().second.push_back(moduleptr->OutPin(j)->net()->pin(z)->module()->No);
                    }
                }
            }
            que.front().second.clear();
            que.front().second.shrink_to_fit();
            que.pop_front();
        }
        for (auto a: moduleOrigin->_outputFF)
        {
            cout << a.second->name() << " ";
        }


}
void Placement::debankFFto1bit(string ffname)
{
    // TODO: need to know which FF should be chosed
    // TODO: Each FF position after debanking
    int celltypeID = 0;
    Module *target = _dataBase->getModuleByName(ffname);
    int ffbit = target->cellType()->getnumBit();
    vector<Module *> newfflist;
    newfflist.clear();
    string pinName;
    // TODO:radius
    vector<pair<double, double>> newPos;
    for (size_t i = 0; i < ffbit; i++)
    {
        newPos.push_back(make_pair(target->x(), target->y()));
        newfflist.push_back(new Module());
        cout << newfflist.size() << endl;
        newfflist[i]->setCellType(_dataBase->ffLib(1, celltypeID));
        newfflist[i]->setPinsize(3);
        newfflist[i]->setInPin(0, target->InPin(i));
        newfflist[i]->InPin(0)->setOffset(newfflist[i]->cellType()->pinOffsetX(1), newfflist[i]->cellType()->pinOffsetY(1));
        newfflist[i]->InPin(0)->setPosition(newfflist[i]->cellType()->pinOffsetX(1) + newPos[i].first,
                                            newfflist[i]->cellType()->pinOffsetY(1) + newPos[i].second);
        pinName = "D";
        newfflist[i]->InPin(0)->setPinName(pinName);
        newfflist[i]->InPin(0)->setModulePtr(newfflist[i]);

        newfflist[i]->setOutPin(0, target->OutPin(i));
        newfflist[i]->OutPin(0)->setOffset(newfflist[i]->cellType()->pinOffsetX(0), newfflist[i]->cellType()->pinOffsetY(0));
        newfflist[i]->OutPin(0)->setPosition(newfflist[i]->cellType()->pinOffsetX(0) + newPos[i].first,
                                             newfflist[i]->cellType()->pinOffsetY(0) + newPos[i].second);
        pinName = "Q";
        newfflist[i]->OutPin(0)->setPinName(pinName);
        newfflist[i]->OutPin(0)->setModulePtr(newfflist[i]);
    }
    newfflist[ffbit - 1]->setName(ffname);
    // naming =================================================================
    string nname = _dataBase->module(_dataBase->getNumModules() - 1)->name();
    string letters;
    string numbers;
    for (char c : nname)
    {
        if (isdigit(c))
        {
            numbers += c;
        }
        else
        {
            letters += c;
        }
    }
    int num = stoi(numbers);
    for (size_t i = 0; i < ffbit - 1; i++)
    {
        num++;
        nname = letters + to_string(num);
        newfflist[i]->setName(nname);
    }
    // naming =================================================================
    newfflist[ffbit - 1]->setInPin(newfflist[ffbit - 1]->cellType()->clkPinIdx(), target->InPin(target->cellType()->clkPinIdx()));
    newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->setModulePtr(newfflist[ffbit - 1]);
    for (size_t i = 0; i < ffbit - 1; i++)
    {
        Pin *newCLK = new Pin();
        string n = "CLK";
        newCLK->setPinName(n);
        newCLK->setOffset(newfflist[i]->cellType()->pinOffsetX(newfflist[i]->cellType()->clkPinIdx()), newfflist[i]->cellType()->pinOffsetY(newfflist[i]->cellType()->clkPinIdx()));
        newCLK->setPosition(newfflist[i]->x() + newCLK->x(), newfflist[i]->y() + newCLK->y());
        newCLK->setModulePtr(newfflist[i]);
        newfflist[i]->setInPin(newfflist[i]->cellType()->clkPinIdx(), newCLK);
        // connecting pin to net
        newCLK->setNetPtr(newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->net());
        newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->net()->addPin(newCLK);
        History *newH = new History();
        newH->setNewPin(newCLK);
        newH->setOldPinName(n);
        newH->setOldModuleName(ffname);
        // add pin into database
        _dataBase->addPin(newCLK);
    }
    for (size_t i = 0; i < ffbit; i++)
    {
        newfflist[i]->setPosition(newPos[i].first, newPos[i].second);
    }
    for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    {
        if (_dataBase->module(i) == target)
        {
            _dataBase->setModule(i, newfflist[ffbit - 1]);
            break;
        }
    }
    for (size_t i = 0; i < _dataBase->getNumFF(); i++)
    {
        if (_dataBase->ff(i) == target)
        {
            _dataBase->setFF(i, newfflist[ffbit - 1]);
            break;
        }
    }
    free(target);
    for (size_t i = 0; i < ffbit - 1; i++)
    {
        _dataBase->addFF(newfflist[i]);
        _dataBase->addModule(newfflist[i]);
    }
    return;
}
// set<set<Module *>> Placement::calMaxClique(unsigned clkidx)
// {
//     // input : clk net id
//     // output : maximal clique(without proper subset)
//     Net *targetNet = _dataBase->net(clkidx);
//     if (targetNet->clkFlag() == false)
//     {
//         cout << "error: input isn't a clk" << endl;
//         return;
//     }
//     ModuleList targetFFs;
//     vector<double> strip; // start pos of every part of strip
//     set<Edge> edgesSet;
//     vector<Edge> edges; // Module, y , start x , end x
//     edges.clear();
//     edgesSet.clear();
//     strip.clear();
//     targetFFs.clear();
//     for (size_t i = 0; i < targetNet->numPins(); i++)
//     {
//         targetFFs.push_back(targetNet->pin(i)->module());
//         strip.push_back(targetFFs[i]->getFeasibleRegion()->left());
//         strip.push_back(targetFFs[i]->getFeasibleRegion()->right());
//         Edge ee;
//         ee.ff = targetFFs[i];
//         ee.y = targetFFs[i]->getFeasibleRegion()->top();
//         ee.isIN = true;
//         ee.startx = targetFFs[i]->getFeasibleRegion()->left();
//         ee.endx = targetFFs[i]->getFeasibleRegion()->right();
//         edgesSet.insert(ee);
//         Edge ee1;
//         ee1.ff = targetFFs[i];
//         ee1.y = targetFFs[i]->getFeasibleRegion()->bottom();
//         ee1.isIN = false;
//         ee1.startx = targetFFs[i]->getFeasibleRegion()->left();
//         ee1.endx = targetFFs[i]->getFeasibleRegion()->right();
//         edgesSet.insert(ee1);
//     }
//     sort(strip.begin(), strip.end());
//     strip.erase(unique(strip.begin(), strip.end()), strip.end());
//     edges.assign(edgesSet.begin(), edgesSet.end());
//     sort(edges.begin(), edges.end(), compareFirst);
//     set<set<Module *>> maxClique;
//     vector<set<Module *>> tempClique; // size == 3 , for checking greater than above and below
//     maxClique.clear();
//     int counter = 0;
//     // TODO: currently traverse every edge per strip, maybe a better O() way to implement
//     for (size_t i = 0; i < strip.size() - 1; i++)
//     {
//         counter = 0;
//         tempClique.clear();
//         tempClique.resize(3);
//         for (size_t j = 0; j < edges.size(); j++)
//         {
//             // sweep line
//             if ((edges[j].startx == strip[i] && edges[j].endx >= strip[i + 1]) ||
//                 (edges[j].startx <= strip[i] && edges[j].endx == strip[i + 1]) ||
//                 (edges[j].startx < strip[i] && edges[j].endx > strip[i + 1]))
//             {
//                 if (counter == 0)
//                 {
//                     // check if Module is in the set

//                     counter++;
//                     if (edges[j].isIN == false)
//                     {
//                         cout << "error" << endl;
//                     }
//                     else
//                     {
//                         tempClique[0].insert(edges[j].ff);
//                     }
//                 }
//                 else if (counter == 1)
//                 {
//                     counter++;
//                     if (edges[j].isIN == false)
//                     {
//                         tempClique[1] = tempClique[0];
//                         tempClique[1].erase(edges[j].ff);
//                         if (tempClique[1].size() > 0)
//                         {
//                             cout << "error" << endl;
//                         }
//                     }
//                 }
//                 else if (counter == 2)
//                 {
//                     counter++;
//                     tempClique[2] = tempClique[1];
//                     if (edges[j].isIN == false)
//                     {
//                         tempClique[2].erase(edges[j].ff);
//                     }
//                     else
//                     {
//                         tempClique[2].insert(edges[j].ff);
//                     }
//                 }
//                 else
//                 {
//                     // move tempClique
//                     if (tempClique[1].size() > tempClique[0].size() &&
//                         tempClique[1].size() > tempClique[2].size() &&
//                         tempClique[1].size() > 1)
//                     {
//                         maxClique.insert(tempClique[1]);
//                     }
//                     tempClique[0] = tempClique[1];
//                     tempClique[1] = tempClique[2];
//                     if (edges[j].isIN == false)
//                     {
//                         tempClique[2].erase(edges[j].ff);
//                     }
//                     else
//                     {
//                         tempClique[2].insert(edges[j].ff);
//                     }
//                 }
//             }
//             else
//             {
//                 continue;
//             }
//         }
//     }
//     // TODO: remove proper subset
//     vector<set<Module *>> toRemove;
//     for (auto it1 = maxClique.begin(); it1 != maxClique.end(); ++it1)
//     {
//         for (auto it2 = maxClique.begin(); it2 != maxClique.end(); ++it2)
//         {
//             if (it1 != it2 && isStrictSubset(*it1, *it2))
//             {
//                 toRemove.push_back(*it1);
//                 break;
//             }
//         }
//     }
//     for (const auto &subset : toRemove)
//     {
//         maxClique.erase(subset);
//     }
//     return maxClique;
// }
bool isStrictSubset(const set<Module *> &a, const set<Module *> &b)
{
    return includes(b.begin(), b.end(), a.begin(), a.end());
}

bool compareFirst(const Edge &a,
                  const Edge &b)
{
    return a.y < b.y;
}
// void Placement::debankFFto1bit(string ffname)
// {
//     // TODO: need to know which FF should be chosed
//     // TODO: Each FF position after debanking
//     int celltypeID = 0;
//     Module *target = _dataBase->getModuleByName(ffname);
//     int ffbit = target->cellType()->getnumBit();
//     vector<Module *> newfflist;
//     newfflist.clear();
//     string pinName;
//     // TODO:radius
//     vector<pair<double, double>> newPos;
//     for (size_t i = 0; i < ffbit; i++)
//     {
//         newPos.push_back(make_pair(target->x(), target->y()));
//         newfflist.push_back(new Module());
//         cout << newfflist.size() << endl;
//         newfflist[i]->setCellType(_dataBase->ffLib(1, celltypeID));
//         newfflist[i]->setPinsize(3);
//         newfflist[i]->setInPin(0, target->InPin(i));
//         newfflist[i]->InPin(0)->setOffset(newfflist[i]->cellType()->pinOffsetX(1), newfflist[i]->cellType()->pinOffsetY(1));
//         newfflist[i]->InPin(0)->setPosition(newfflist[i]->cellType()->pinOffsetX(1) + newPos[i].first,
//                                             newfflist[i]->cellType()->pinOffsetY(1) + newPos[i].second);
//         pinName = "D";
//         newfflist[i]->InPin(0)->setPinName(pinName);
//         newfflist[i]->InPin(0)->setModulePtr(newfflist[i]);

//         newfflist[i]->setOutPin(0, target->OutPin(i));
//         newfflist[i]->OutPin(0)->setOffset(newfflist[i]->cellType()->pinOffsetX(0), newfflist[i]->cellType()->pinOffsetY(0));
//         newfflist[i]->OutPin(0)->setPosition(newfflist[i]->cellType()->pinOffsetX(0) + newPos[i].first,
//                                              newfflist[i]->cellType()->pinOffsetY(0) + newPos[i].second);
//         pinName = "Q";
//         newfflist[i]->OutPin(0)->setPinName(pinName);
//         newfflist[i]->OutPin(0)->setModulePtr(newfflist[i]);
//     }
//     newfflist[ffbit - 1]->setName(ffname);
//     // naming =================================================================
//     string nname = _dataBase->module(_dataBase->getNumModules() - 1)->name();
//     string letters;
//     string numbers;
//     for (char c : nname)
//     {
//         if (isdigit(c))
//         {
//             numbers += c;
//         }
//         else
//         {
//             letters += c;
//         }
//     }
//     int num = stoi(numbers);
//     for (size_t i = 0; i < ffbit - 1; i++)
//     {
//         num++;
//         nname = letters + to_string(num);
//         newfflist[i]->setName(nname);
//     }
//     // naming =================================================================
//     newfflist[ffbit - 1]->setInPin(newfflist[ffbit - 1]->cellType()->clkPinIdx(), target->InPin(target->cellType()->clkPinIdx()));
//     newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->setModulePtr(newfflist[ffbit - 1]);
//     for (size_t i = 0; i < ffbit - 1; i++)
//     {
//         Pin *newCLK = new Pin();
//         string n = "CLK";
//         newCLK->setPinName(n);
//         newCLK->setOffset(newfflist[i]->cellType()->pinOffsetX(newfflist[i]->cellType()->clkPinIdx()), newfflist[i]->cellType()->pinOffsetY(newfflist[i]->cellType()->clkPinIdx()));
//         newCLK->setPosition(newfflist[i]->x() + newCLK->x(), newfflist[i]->y() + newCLK->y());
//         newCLK->setModulePtr(newfflist[i]);
//         newfflist[i]->setInPin(newfflist[i]->cellType()->clkPinIdx(), newCLK);
//         // connecting pin to net
//         newCLK->setNetPtr(newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->net());
//         newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->net()->addPin(newCLK);
//         History *newH = new History();
//         newH->setNewPin(newCLK);
//         newH->setOldPinName(n);
//         newH->setOldModuleName(ffname);
//         // add pin into database
//         _dataBase->addPin(newCLK);
//     }
//     for (size_t i = 0; i < ffbit; i++)
//     {
//         newfflist[i]->setPosition(newPos[i].first, newPos[i].second);
//     }
//     for (size_t i = 0; i < _dataBase->getNumModules(); i++)
//     {
//         if (_dataBase->module(i) == target)
//         {
//             _dataBase->setModule(i, newfflist[ffbit - 1]);
//             break;
//         }
//     }
//     for (size_t i = 0; i < _dataBase->getNumFF(); i++)
//     {
//         if (_dataBase->ff(i) == target)
//         {
//             _dataBase->setFF(i, newfflist[ffbit - 1]);
//             break;
//         }
//     }
//     free(target);
//     for (size_t i = 0; i < ffbit - 1; i++)
//     {
//         _dataBase->addFF(newfflist[i]);
//         _dataBase->addModule(newfflist[i]);
//     }
//     return;
// }
// set<set<Module *>> Placement::calMaxClique(unsigned clkidx)
// {
//     // input : clk net id
//     // output : maximal clique(without proper subset)
//     Net *targetNet = _dataBase->net(clkidx);
//     if (targetNet->clkFlag() == false)
//     {
//         cout << "error: input isn't a clk" << endl;
//         return;
//     }
//     ModuleList targetFFs;
//     vector<double> strip; // start pos of every part of strip
//     set<Edge> edgesSet;
//     vector<Edge> edges; // Module, y , start x , end x
//     edges.clear();
//     edgesSet.clear();
//     strip.clear();
//     targetFFs.clear();
//     for (size_t i = 0; i < targetNet->numPins(); i++)
//     {
//         targetFFs.push_back(targetNet->pin(i)->module());
//         strip.push_back(targetFFs[i]->getFeasibleRegion()->left());
//         strip.push_back(targetFFs[i]->getFeasibleRegion()->right());
//         Edge ee;
//         ee.ff = targetFFs[i];
//         ee.y = targetFFs[i]->getFeasibleRegion()->top();
//         ee.isIN = true;
//         ee.startx = targetFFs[i]->getFeasibleRegion()->left();
//         ee.endx = targetFFs[i]->getFeasibleRegion()->right();
//         edgesSet.insert(ee);
//         Edge ee1;
//         ee1.ff = targetFFs[i];
//         ee1.y = targetFFs[i]->getFeasibleRegion()->bottom();
//         ee1.isIN = false;
//         ee1.startx = targetFFs[i]->getFeasibleRegion()->left();
//         ee1.endx = targetFFs[i]->getFeasibleRegion()->right();
//         edgesSet.insert(ee1);
//     }
//     sort(strip.begin(), strip.end());
//     strip.erase(unique(strip.begin(), strip.end()), strip.end());
//     edges.assign(edgesSet.begin(), edgesSet.end());
//     sort(edges.begin(), edges.end(), compareFirst);
//     set<set<Module *>> maxClique;
//     vector<set<Module *>> tempClique; // size == 3 , for checking greater than above and below
//     maxClique.clear();
//     int counter = 0;
//     // TODO: currently traverse every edge per strip, maybe a better O() way to implement
//     for (size_t i = 0; i < strip.size() - 1; i++)
//     {
//         counter = 0;
//         tempClique.clear();
//         tempClique.resize(3);
//         for (size_t j = 0; j < edges.size(); j++)
//         {
//             // sweep line
//             if ((edges[j].startx == strip[i] && edges[j].endx >= strip[i + 1]) ||
//                 (edges[j].startx <= strip[i] && edges[j].endx == strip[i + 1]) ||
//                 (edges[j].startx < strip[i] && edges[j].endx > strip[i + 1]))
//             {
//                 if (counter == 0)
//                 {
//                     // check if Module is in the set

//                     counter++;
//                     if (edges[j].isIN == false)
//                     {
//                         cout << "error" << endl;
//                     }
//                     else
//                     {
//                         tempClique[0].insert(edges[j].ff);
//                     }
//                 }
//                 else if (counter == 1)
//                 {
//                     counter++;
//                     if (edges[j].isIN == false)
//                     {
//                         tempClique[1] = tempClique[0];
//                         tempClique[1].erase(edges[j].ff);
//                         if (tempClique[1].size() > 0)
//                         {
//                             cout << "error" << endl;
//                         }
//                     }
//                 }
//                 else if (counter == 2)
//                 {
//                     counter++;
//                     tempClique[2] = tempClique[1];
//                     if (edges[j].isIN == false)
//                     {
//                         tempClique[2].erase(edges[j].ff);
//                     }
//                     else
//                     {
//                         tempClique[2].insert(edges[j].ff);
//                     }
//                 }
//                 else
//                 {
//                     // move tempClique
//                     if (tempClique[1].size() > tempClique[0].size() &&
//                         tempClique[1].size() > tempClique[2].size() &&
//                         tempClique[1].size() > 1)
//                     {
//                         maxClique.insert(tempClique[1]);
//                     }
//                     tempClique[0] = tempClique[1];
//                     tempClique[1] = tempClique[2];
//                     if (edges[j].isIN == false)
//                     {
//                         tempClique[2].erase(edges[j].ff);
//                     }
//                     else
//                     {
//                         tempClique[2].insert(edges[j].ff);
//                     }
//                 }
//             }
//             else
//             {
//                 continue;
//             }
//         }
//     }
//     // TODO: remove proper subset
//     vector<set<Module *>> toRemove;
//     for (auto it1 = maxClique.begin(); it1 != maxClique.end(); ++it1)
//     {
//         for (auto it2 = maxClique.begin(); it2 != maxClique.end(); ++it2)
//         {
//             if (it1 != it2 && isStrictSubset(*it1, *it2))
//             {
//                 toRemove.push_back(*it1);
//                 break;
//             }
//         }
//     }
//     for (const auto &subset : toRemove)
//     {
//         maxClique.erase(subset);
//     }
//     return maxClique;
// }
// bool isStrictSubset(const set<Module *> &a, const set<Module *> &b)
// {
//     return includes(b.begin(), b.end(), a.begin(), a.end());
// }

// bool compareFirst(const Edge &a,
//                   const Edge &b)
// {
//     return a.y < b.y;
// }