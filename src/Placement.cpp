#include "Placement.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>
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
    if (static_cast<double>(leafsize) / static_cast<double>(_nodes.size()) < leafthresold) // leaf too much
    {                                                                                      // do merge & store mst into _nodes
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
        pair.second.first->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
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
    newffidx = rand() % _dataBase->getNumfflibBit((m1->cellType()->getnumBit()) * 2);
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
    // print MST///////////////////////////////////////////////
    // for (size_t i = 0; i < mst.size(); i++)
    // {
    //     cout << "Node " << mst[i]->getFFinNode()->name() << " has neighbor: ";
    //     cout << " " << mst[i]->getNodeidxheap() << endl;
    //     map<string, pair<Node *, double>> neighbor = mst[i]->getneighbormap();
    //     for (const auto &pair : neighbor)
    //     {
    //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //     }
    //     cout << endl;
    // }
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

double cal_distance(pair<double, double> A, pair<double, double> B) {
    return sqrt(pow(A.first - B.first, 2) + pow(A.second - B.second, 2));
}

bool overlap_ornot(Module *a, double a_r, Module *b, double b_r) {
    pair<double, double> b_cor = {b->centerX(), b->centerY()};
    pair<double, double> cor;

    cor = {a->centerX() + a_r, a->centerY()};
    if (cal_distance(cor, b_cor) <= b_r)
        return true;

    cor = {a->centerX() - a_r, a->centerY()};
    if (cal_distance(cor, b_cor) <= b_r)
        return true;

    cor = {a->centerX(), a->centerY() + a_r};
    if (cal_distance(cor, b_cor) <= b_r)
        return true;

    cor = {a->centerX(), a->centerY() - a_r};
    if (cal_distance(cor, b_cor) <= b_r)
        return true;

    return false;
}

void Placement::constructGraph() {
    int num_node = _diamondINF.size();
    for (int i = 0; i < num_node; ++i) {
        Node *n = new Node;
        n->setFFinNode(_diamondINF[i]);
        _name2Node[n->getFFinNode()->name()] = n;
        _nodes.push_back(n);
    }
    // Module *test =_dataBase->getClkNets()[0]->pin(0).module();
    for (int i = 0; i < _dataBase->getNumClkNets(); ++i) {
        for (int j = 0; j < _dataBase->getClkNets()[i]->numPins(); ++j) {
            if (_dataBase->getClkNets()[i]->pin(j)->isIOdie() == 1)  // IO pin doesn't map module
                continue;
            if (_dataBase->getClkNets()[i]->pin(j)->module()->isFF() == 0)  // we only can merge FF
                continue;
            else {
                Module *module_i_j = _dataBase->getClkNets()[i]->pin(j)->module();
                for (int k = j + 1; k < _dataBase->getClkNets()[i]->numPins(); ++k) {
                    Module *module_i_k = _dataBase->getClkNets()[i]->pin(k)->module();
                    if (module_i_k->isFF() == 0)
                        continue;
                    else if (overlap_ornot(module_i_j, module_i_j->radius(), module_i_k, module_i_k->radius())) {  // they are overlapping and in the same clknet
                        _name2Node[module_i_j->name()]->addNeighborPair({_name2Node[module_i_k->name()], 0});      // weight is 0(temporary)
                    }
                }
            }
        }
    }
}