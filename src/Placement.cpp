#include "Placement.h"
// #include <mutex> // threads header file but cannot use in C++98
// #include <thread>
#include <cctype>
#include <cmath>
#include <cstdlib>
// #include <ctime>
#include <limits.h>
#include <vector>
// #include <random>
#include <set>
#include <math.h>
#include <set>
#include <cfloat>
#include <queue>
#include <utility>
using namespace std;

#define leafthresold 0.75 // TODO: can be changed
// #define __DBL_MAX__ 1.7976931348623158e+308 /* max value */
struct Edges
{
    Module *ff;
    double y;
    double startx;
    double endx;
    bool isIN;
    bool operator<(const Edges &other) const
    {
        return y < other.y;
    }
};
typedef struct Edges Edge;
void Placement::mainLoop()
{
    //    // find same clk path-> calculate diamond -> construct graph according to cost -> find MST -> merge FFs
    //    // TODO:how to find same clk path , and loop it
    //    NodeList mst = findMST();
    //    // print MST ///////////////////////////////////////////////
    //    for (size_t i = 0; i < mst.size(); i++)
    //    {
    //        cout << "Node " << mst[i]->getFFinNode()->name() << " has neighbor: ";
    //        map<string, pair<Node *, double> > neighbor = mst[i]->getneighbormap();
    //        //for (const auto &pair : neighbor)
    //        //{
    //        //    cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //        //}
    //        //cout << endl;
    //        map<string, pair<Node*, double> >::const_iterator it;
    //        for (it = neighbor.begin(); it != neighbor.end(); ++it)
    //        {
    //            std::cout << it->second.first->getFFinNode()->name() << " " << it->second.second << " ";
    //        }
    //        std::cout << std::endl;

    //    }
    //    // print MST ///////////////////////////////////////////////
    //    int leafsize = 0;
    //    for (size_t i = 0; i < mst.size(); i++)
    //    {
    //        if (mst[i]->getNeighborsize() == 1)
    //        {
    //            mst[i]->setisleaf(true);
    //            leafsize++;
    //        }
    //        else
    //        {
    //            mst[i]->setisleaf(false);
    //        }
    //    }
    //    if (static_cast<double>(leafsize) / static_cast<double>(_nodes.size()) < leafthresold) // leaf not too much
    //    {                                                                                      // do merge & store mst into _nodes
    //        cout << "do merge on mst\n";
    //        for (size_t i = 0; i < _nodes.size(); i++)
    //        {
    //            free(_nodes[i]);
    //        }
    //        _nodes.clear();
    //        _nodes = mst;
    //        mergeFFinG();
    //    }
    //    else // greedy pick to merge leaf too much
    //    {
    //        cout << "leaf too much, merge randomly\n";
    //        for (size_t i = 0; i < mst.size(); i++)
    //        {
    //            free(mst[i]);
    //        }
    //        mst.clear();
    //        cout << "node size : " << _nodes.size() << endl;
    //        while (_nodes.size() > 1)
    //        {
    //            random_device rd;
    //            mt19937 generator(rd());
    //            uniform_int_distribution<unsigned> distribution(0, _nodes.size() - 1);
    //            unsigned target = distribution(generator);
    //            cout << "target : " << target << endl;
    //            while (_nodes[target]->getNeighborsize() < 1)
    //            {
    //                target = distribution(generator);
    //            }
    //            //map<string, pair<Node *, double> > neighbor = _nodes[target]->getneighbormap();
    //            //string maxidx = neighbor.begin()->first;
    //            //for (const auto &pair : neighbor)
    //            //{
    //            //    if (pair.second.second > neighbor[maxidx].second)
    //            //    {
    //            //        maxidx = pair.first;
    //            //    }
    //            //}
    //            map<std::string, pair<Node*, double> > neighbor = _nodes[target]->getneighbormap();
    //            string maxidx = neighbor.begin()->first;
    //            for (map<std::string, pair<Node*, double> >::const_iterator it = neighbor.begin(); it != neighbor.end(); ++it)
    //            {
    //                if (it->second.second > neighbor[maxidx].second)
    //                {
    //                    maxidx = it->first;
    //                }
    //            }

    //            // TODO: maybe randomly pick which FF to merge to ??
    //            merge2FF(target, neighbor[maxidx].first->getNodeidxheap(), 0);
    //        }
    //    }
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
        // auto mymap = *_nodes[idx]->getneighbormap().begin();
        // clang-format off
        map<string, pair<Node *, double> >::value_type mymap = *_nodes[idx]->getneighbormap().begin();
        // clang-format on
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
    vector<Node *>::iterator it = _nodes.begin();
    for (; it != _nodes.end(); ++it)
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

int my_stoi(const std::string &str)
{
    int num = 0;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            num = num * 10 + (str[i] - '0');
        }
        else
        {
            return 0;
        }
    }
    return num;
}

string my_itos(int num)
{
    string ans = "";
    while (num)
    {
        ans = char((num % 10) + '0') + ans;
        num /= 10;
    }
    return ans;
}

void Placement::merge2FF(unsigned idx1, unsigned idx2, unsigned newffidx)
{
    // // TODO: need to know which FF should be chosed
    // // ex: merge two 1 bit, which 2 bit FF should be chosed?
    // // erase to FF from graph(_nodes)/////////////////////////////////
    // // clang-format off
    // map<string, pair<Node *, double> > neighbor = _nodes[idx1]->getneighbormap();
    // for (const auto &pair : neighbor)
    // {
    //     pair.second.first->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
    // }
    // neighbor = _nodes[idx2]->getneighbormap();
    // for (const auto &pair : neighbor)
    // {
    //     pair.second.first->eraseNeighbor(_nodes[idx2]->getFFinNode()->name());
    // }
    // map<string, pair<Node *, double> > neighbor = _nodes[idx1]->getneighbormap();
    // for (map<std::string, pair<Node *, double> >::iterator it = neighbor.begin(); it != neighbor.end(); ++it)
    // {
    //     it->second.first->eraseNeighbor(_nodes[idx1]->getFFinNode()->name());
    // }

    // neighbor = _nodes[idx2]->getneighbormap();
    // for (map<string, pair<Node *, double> >::iterator it = neighbor.begin(); it != neighbor.end(); ++it)
    // {
    //     it->second.first->eraseNeighbor(_nodes[idx2]->getFFinNode()->name());
    // }
    // // clang-format on
    // _nodes[idx1]->clearNeighbor();
    // _nodes[idx2]->clearNeighbor();
    // // erase to FF from graph(_nodes)/////////////////////////////////
    // Module *m1 = _nodes[idx1]->getFFinNode();
    // Module *m2 = _nodes[idx2]->getFFinNode();
    // // cal position///////////////////////////////////////////////
    // double m1x, m1y, m2x, m2y;
    // double newX, newY;
    // m1x = m1->centerX();
    // m1y = m1->centerY();
    // m2x = m2->centerX();
    // m2y = m2->centerY();
    // // m1->setRadius(3000);
    // // m2->setRadius(3000);
    // Rhombus *r1 = new Rhombus(m1x, m1y, m1->radius());
    // Rhombus *r2 = new Rhombus(m2x, m2y, m2->radius());
    // pair<double, double> newloc = r1->findCentroidIntersect(*r1, *r2);
    // newX = newloc.first;
    // newY = newloc.second;
    // // cal position///////////////////////////////////////////////
    // string m3name = _dataBase->module(_dataBase->getNumModules() - 1)->name();
    // string letters;
    // string numbers;
    // for (char c : m3name)
    // {
    //     if (std::isdigit(c))
    //     {
    //         numbers += c;
    //     }
    //     else
    //     {
    //         letters += c;
    //     }
    // }
    // for (string::size_type i = 0; i < m3name.size(); ++i)
    // {
    //     char c = m3name[i];
    //     if (std::isdigit(static_cast<unsigned char>(c)))
    //     {
    //         numbers += c;
    //     }
    //     else
    //     {
    //         letters += c;
    //     }
    // }
    // int num = my_stoi(numbers);
    // m3name = letters + my_itos(num + 1);
    // Module *m3 = new Module(m3name, _dataBase->getBestCelltype((m1->cellType()->getnumBit()) * 2), newX, newY);
    // m3->clearPins();
    // m3->setPinsize(m3->cellType()->getnumBit() * 2 + 1);
    // int pinid = 0;
    // for (size_t i = 0; i < m1->numInPins(); i++)
    // {
    //     if (m1->InPin(i)->name() != "CLK" && m1->OutPin(i)->name() != "clk")
    //     {
    //         string str = "D";
    //         str = str + my_itos(pinid);
    //         m1->InPin(i)->setPinName(str);
    //         pinid++;
    //     }
    // }
    // for (size_t i = 0; i < m2->numInPins(); i++)
    // {
    //     if (m2->InPin(i)->name() != "CLK" && m2->OutPin(i)->name() != "clk")
    //     {
    //         string str = "D";
    //         str = str + my_itos(pinid);
    //         m2->InPin(i)->setPinName(str);
    //         pinid++;
    //     }
    // }
    // pinid = 0;
    // for (size_t i = 0; i < m1->numOutPins(); i++)
    // {
    //     if (m1->OutPin(i)->name() != "CLK" && m1->OutPin(i)->name() != "clk")
    //     {
    //         string str = "Q";
    //         str = str + my_itos(pinid);
    //         m1->OutPin(i)->setPinName(str);
    //         pinid++;
    //     }
    // }
    // for (size_t i = 0; i < m2->numOutPins(); i++)
    // {
    //     if (m2->OutPin(i)->name() != "CLK" && m2->OutPin(i)->name() != "clk")
    //     {
    //         string str = "Q";
    //         str = str + my_itos(pinid);
    //         m2->OutPin(i)->setPinName(str);
    //         pinid++;
    //     }
    // }
    // for (size_t i = 0; i < m3->cellType()->getnumBit() / 2; i++)
    // {
    //     m3->setInPin(i, m1->InPin(i));
    //     m3->InPin(i)->setModulePtr(m3);
    // }
    // for (size_t i = m3->cellType()->getnumBit() / 2; i < m3->cellType()->getnumBit(); i++)
    // {
    //     m3->setInPin(i, m2->InPin(i - m3->cellType()->getnumBit() / 2));
    //     m3->InPin(i)->setModulePtr(m3);
    // }
    // for (size_t i = 0; i < m3->cellType()->getnumBit() / 2; i++)
    // {
    //     m3->setOutPin(i, m1->OutPin(i));
    //     m3->OutPin(i)->setModulePtr(m3);
    // }
    // for (size_t i = m3->cellType()->getnumBit() / 2; i < m3->cellType()->getnumBit(); i++)
    // {
    //     m3->setOutPin(i, m2->OutPin(i - m3->cellType()->getnumBit() / 2));
    //     m3->OutPin(i)->setModulePtr(m3);
    // }
    // m3->setInPin(m3->cellType()->clkPinIdx(), m1->InPin(m1->cellType()->clkPinIdx()));
    // m3->InPin(m3->cellType()->clkPinIdx())->setModulePtr(m3);
    // // update Module in Pin
    // // update x,y offset
    // // TODO: may be wrong here
    // for (size_t i = 0; i < m3->totnumPins(); i++)
    // {
    //     m3->pin(i)->setOffset(m3->cellType()->pinOffsetX(i), m3->cellType()->pinOffsetY(i));
    //     m3->pin(i)->setPosition(m3->x() + m3->pin(i)->xOffset(), m3->y() + m3->pin(i)->yOffset());
    // }
    // //  so D0~Dbits/2 is m1's old pin , and we keep m1's CLK pin
    // for (size_t i = 0; i < m2->InPin(m2->cellType()->clkPinIdx())->net()->numPins(); i++)
    // {
    //     if (m2->InPin(m2->cellType()->clkPinIdx())->net()->pin(i) == m2->InPin(m2->cellType()->clkPinIdx()))
    //     {
    //         m2->InPin(m2->cellType()->clkPinIdx())->net()->erasePin(i);
    //         break;
    //     }
    // }
    // for (size_t i = 0; i < _dataBase->getNumPins(); i++)
    // {
    //     if (m2->InPin(m2->cellType()->clkPinIdx()) == _dataBase->pin(i))
    //     {
    //         _dataBase->erasePin(i);
    //         break;
    //     }
    // }
    // Pin *p = m2->InPin(m2->cellType()->clkPinIdx());
    // free(p->history());
    // free(p);
    // //  clear FFs with no neighbor out of graph(_nodes)/////////////////////////////////
    // // free m1 and m2
    // for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    // {
    //     if (_dataBase->module(i) == m1)
    //     {
    //         _dataBase->eraseModule(i);
    //         break;
    //     }
    // }
    // for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    // {
    //     if (_dataBase->module(i) == m2)
    //     {
    //         _dataBase->eraseModule(i);
    //         break;
    //     }
    // }
    // for (size_t i = 0; i < _dataBase->getNumFF(); i++)
    // {
    //     if (_dataBase->ff(i) == m1)
    //     {
    //         _dataBase->eraseFF(i);
    //         break;
    //     }
    // }
    // for (size_t i = 0; i < _dataBase->getNumFF(); i++)
    // {
    //     if (_dataBase->ff(i) == m2)
    //     {
    //         _dataBase->eraseFF(i);
    //         break;
    //     }
    // }

    // free(m1);
    // free(m2);
    // _dataBase->addModule(m3);
    // _dataBase->addFF(m3);
    // m3->setCenterPosition(newX, newY);
    // // check boundary
    // if (m3->x() < _dataBase->getBoundaryLeft())
    // {
    //     m3->setPosition(_dataBase->getBoundaryLeft(), m3->y());
    // }
    // if (m3->y() < _dataBase->getBoundaryBottom())
    // {
    //     m3->setPosition(m3->x(), _dataBase->getBoundaryBottom());
    // }
    // if (m3->x() + m3->width() > _dataBase->getBoundaryRight())
    // {
    //     m3->setPosition(_dataBase->getBoundaryRight() - m3->width(), m3->y());
    // }
    // if (m3->y() + m3->height() > _dataBase->getBoundaryTop())
    // {
    //     m3->setPosition(m3->x(), _dataBase->getBoundaryTop() - m3->height());
    // }

    // // delete nodes with no neighbor
    // std::vector<Node *>::iterator it = _nodes.begin();
    // for (; it != _nodes.end(); ++it)
    // {
    //     if ((*it)->getNeighborsize() == 0)
    //     {
    //         delete *it;
    //         it = _nodes.erase(it);
    //     }
    //     else
    //     {
    //         (*it)->setNodeidxheap(it - _nodes.begin());
    //         ++it;
    //     }
    // }
    // return;
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
    // clang-format off
    vector<pair<Node *, pair<double, Node *> > > qheap; // <Node,key,predecessor>
    qheap.resize(_nodes.size());
    for (size_t i = 0; i < _nodes.size(); i++)
    {
        _nodes[i]->setNodeidxheap(i);
        qheap[i].first = _nodes[i];
        qheap[i].second.first = __DBL_MAX__;
        qheap[i].second.second = NULL;
    }
    qheap[0].second.first = 0;
    vector<pair<Node *, pair<double, Node *> > > mstHeap;
    NodeList mst;
    while (qheap.size() > 0)
    {
        // pair<Node *, pair<double, Node *> > min = extractMinMST(qheap);
        // min.first->setNodeidxheap(-1);
        // map<string, pair<Node *, double> > neighbor = min.first->getneighbormap(); // previous node's neighbor
        // for (map<string, pair<Node*, double> >::iterator it = neighbor.begin(); it != neighbor.end(); ++it)
        //{
        //     if (pair.second.first->getNodeidxheap() != -1)
        //     {
        //         if (pair.second.second < qheap[pair.second.first->getNodeidxheap()].second.first)
        //         {
        //             qheap[pair.second.first->getNodeidxheap()].second.second = min.first; // set predecessor
        //             DecreaseKeyMST(qheap, pair.second.first->getNodeidxheap(), pair.second.second);
        //         }
        //     }
        pair<Node *, pair<double, Node *> > min = extractMinMST(qheap);
        min.first->setNodeidxheap(-1);
        map<string, pair<Node *, double> > neighbor = min.first->getneighbormap(); // previous node's neighbor
        for (map<string, pair<Node *, double> >::iterator it = neighbor.begin(); it != neighbor.end(); ++it)
        {
            if (it->second.first->getNodeidxheap() != -1)
            {
                if (it->second.second < qheap[it->second.first->getNodeidxheap()].second.first)
                {
                    qheap[it->second.first->getNodeidxheap()].second.second = min.first; // set predecessor
                    DecreaseKeyMST(qheap, it->second.first->getNodeidxheap(), it->second.second);
                }
            }
        }
        mstHeap.push_back(min);
    } // clang-format on
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
    //     map<string, pair<Node *, double> > neighbor = mst[i]->getneighbormap();
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
    //     map<string, pair<Node *, double> > neighbor = qheap[i].first->getneighbormap();
    //     for (const auto &pair : neighbor)
    //     {
    //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //     }
    //     cout << endl;
    // }
    // print the heap//////////////////////////////////////////
}
// clang-format off
void Placement::DecreaseKeyMST(vector<pair<Node *, pair<double, Node *> > > &heap, unsigned idx, double key)
// clang-format on
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
// clang-format off
pair<Node *, pair<double, Node *> > Placement::extractMinMST(vector<pair<Node *, pair<double, Node *> > > &heap)
{
    pair<Node *, pair<double, Node *> > n = heap[0];
    // clang-format on
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
// clang-format off
void Placement::swapNodeMST(vector<pair<Node *, pair<double, Node *> > > &heap, unsigned idx1, unsigned idx2)
{
    pair<Node *, pair<double, Node *> > temp;
    // clang-format on
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

bool overlap_ornot(std::vector<Rhombus *> &input_rhombus, double &leftBound, double &rightBound, double &botBound, double &topBound)
{
    if (input_rhombus.empty())
        return false;

    leftBound = -DBL_MAX; // 使用 -DBL_MAX 表示最小可能值
    rightBound = DBL_MAX; // 使用 DBL_MAX 表示最大可能值
    botBound = -DBL_MAX;  // 使用 -DBL_MAX 表示最小可能值
    topBound = DBL_MAX;   // 使用 DBL_MAX 表示最大可能值

    // 旋转 45 度
    for (std::vector<Rhombus *>::iterator it = input_rhombus.begin(); it != input_rhombus.end(); ++it)
    {
        (*it)->RotatePeak(45); // 解引用指针调用方法
    }

    for (std::vector<Rhombus *>::iterator it = input_rhombus.begin(); it != input_rhombus.end(); ++it)
    {
        leftBound = std::max(leftBound, (*it)->peak(3).x);
        rightBound = std::min(rightBound, (*it)->peak(1).x);
        botBound = std::max(botBound, (*it)->peak(3).y);
        topBound = std::min(topBound, (*it)->peak(1).y);
    }

    // 旋转回 -45 度
    for (std::vector<Rhombus *>::iterator it = input_rhombus.begin(); it != input_rhombus.end(); ++it)
    {
        (*it)->RotatePeak(-45); // 解引用指针调用方法
    }

    // 检查菱形是否重叠
    if ((leftBound > rightBound) || (botBound > topBound))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool feasibleRegion_overlap(const Rectangle *a, const Rectangle *b)
{
    bool xoverlap = abs(a->getX1() - b->getX1()) < (a->getWidth() / 2 + b->getWidth() / 2);
    bool yoverlap = abs(a->getY1() - b->getY1()) < (a->getHeight() / 2 + b->getHeight() / 2);
    return xoverlap && yoverlap;
}

void Placement::windows() // construct weilun graph
{
    double chip_L = _dataBase->getBoundaryLeft();
    double chip_R = _dataBase->getBoundaryRight();
    double chip_B = _dataBase->getBoundaryBottom();
    double chip_T = _dataBase->getBoundaryTop();
    double window_size = sqrt((chip_T - chip_B) * (chip_L - chip_R)); // we can adjust window size
    // double step_size = window_size / 2;
    // clang-format off
    map<pair<double, double>, vector<Module *> > window_idx2FF;
    // clang-format on
    _nodes.clear();
    int num_FF = _dataBase->getNumFF();
    for (int i = 0; i < num_FF; ++i)
    {
        // Create a new node for the current FF.
        Node *n = new Node;
        n->setFFinNode(_dataBase->ff(i));
        _name2Node[n->getFFinNode()->name()] = n;
        _nodes.push_back(n);

        // Assign a window index to the current FF.
        double idx_x = _dataBase->ff(i)->x() / window_size;
        double idx_y = _dataBase->ff(i)->y() / window_size;
        window_idx2FF[make_pair(idx_x, idx_y)].push_back(_dataBase->ff(i));

        // Construct feasible region for current FF
        constructFeasible(_dataBase->ff(i));
    }
    double y_min = chip_B / window_size;
    double y_max = chip_T / window_size;
    double x_min = chip_L / window_size;
    double x_max = chip_R / window_size;
    // for (int y = y_min; y <= y_max; ++y)
    // {
    //     for (int x = x_min; x <= x_max; ++x)
    //     {
    // double length = window_idx2FF[{x, y}].size();
    double length = num_FF;
    for (int i = 0; i < length; ++i)
    {
        for (int j = i + 1; j < length; ++j)
        {
            if (_dataBase->ff(i)->getFeasibleRegion() == NULL)
                continue;
            if (_dataBase->ff(j)->getFeasibleRegion() == NULL)
                continue;
            if (feasibleRegion_overlap(_dataBase->ff(i)->getFeasibleRegion(), _dataBase->ff(j)->getFeasibleRegion()))
            {
                ;
                _name2Node[_dataBase->ff(i)->name()]->addNeighborPair(make_pair(_name2Node[_dataBase->ff(j)->name()], 0)); // without cost
                _name2Node[_dataBase->ff(j)->name()]->addNeighborPair(make_pair(_name2Node[_dataBase->ff(i)->name()], 0)); // two directions
                // cout << "node " << _dataBase->ff(i)->name() << "has neighbor " << _dataBase->ff(j)->name() << endl;
            }
        }
    }
    //     }
    // }
}
// double Placement::getDen()
// {
//     double den = 0;
//     for(int i = 0;i<_dataBase->getbincol();++i)
//     {
//         for(int j = 0;j<_dataBase->getbinrow();++j)
//         {
//             _dataBase->bin(i,j)
//         }
//     }
// }
double Placement::cal_total_cost()
{
    double cost = 0;
    double alpha = _dataBase->getAlpha();
    double beta = _dataBase->getBeta();
    double gamma = _dataBase->getGamma();
    double lambda = _dataBase->getLambda();

    double tns = 0;
    double power = 0;
    double area = 0;

    for (int i = 0; i < _dataBase->getNumFF(); ++i)
    {
        tns = _dataBase->ff(i)->getTNS();
        power = _dataBase->ff(i)->getPower();
        area = _dataBase->ff(i)->area();

        cost += alpha * tns + beta * power + gamma * area;
        tns = 0;
        power = 0;
        area = 0;
    }
    // cost += lambda * binutil;

    return cost;
}

Rhombus *Placement::findInputRegion(Module *ff)
{
    // ff must be 1 bit FF
    if (ff->InPin(0)->net()->OutputPin()->module() == NULL) // directory to input pin
    {
        Rhombus *ans = new Rhombus(ff->InPin(0)->net()->OutputPin()->x(), ff->InPin(0)->net()->OutputPin()->y(), 0);
        return ans;
    }
    else
    {
        double ff_original_slack = ff->InPin(0)->getSlackInfor()->slack();
        double dis_delay = _dataBase->getDisplacementDelay();
        double WL_D_N = abs(ff->InPin(0)->net()->OutputPin()->x() - ff->InPin(0)->x()) + abs(ff->InPin(0)->net()->OutputPin()->y() - ff->InPin(0)->y());
        double radius = (ff_original_slack + dis_delay * WL_D_N) / dis_delay;
        Rhombus *ans = new Rhombus(ff->InPin(0)->net()->OutputPin()->x(), ff->InPin(0)->net()->OutputPin()->y(), radius);
        return ans;
    }
    // ff must be 1 bit FF
    if (ff->InPin(0)->net()->OutputPin()->module() == NULL) // directory to input pin
    {
        Rhombus *ans = new Rhombus(ff->InPin(0)->net()->OutputPin()->x(), ff->InPin(0)->net()->OutputPin()->y(), 0);
        return ans;
    }
    else
    {
        double ff_original_slack = ff->InPin(0)->getSlackInfor()->slack();
        double dis_delay = _dataBase->getDisplacementDelay();
        double WL_D_N = abs(ff->InPin(0)->net()->OutputPin()->x() - ff->InPin(0)->x()) + abs(ff->InPin(0)->net()->OutputPin()->y() - ff->InPin(0)->y());
        double radius = (ff_original_slack + dis_delay * WL_D_N) / dis_delay;
        Rhombus *ans = new Rhombus(ff->InPin(0)->net()->OutputPin()->x(), ff->InPin(0)->net()->OutputPin()->y(), radius);
        return ans;
    }
}
vector<Rhombus *> Placement::findOutputRegion(Module *ff)
{
    // ff must be 1 bit FF
    /*by GPT , must check */
    vector<Rhombus *> multi_region;
    // clang-format off
    for (set<pair<Module *, Module *> >::iterator it = ff->_outputFF.begin(); it != ff->_outputFF.end(); ++it)
    { // clang-format on
        double slack = it->second->InPin(0)->slack();
        double dis_delay = _dataBase->getDisplacementDelay();
        double WL_Q_0;

        if (it->first == NULL)
        {
            WL_Q_0 = abs(ff->OutPin(0)->x() - it->second->InPin(0)->x()) +
                     abs(ff->OutPin(0)->y() - it->second->InPin(0)->y());
            double radius = (slack + dis_delay * WL_Q_0) / dis_delay;
            Rhombus *ans = new Rhombus(it->second->InPin(0)->x(), it->second->InPin(0)->y(), radius);
            multi_region.push_back(ans);
        }
        else
        { // curr_FF 和 out_FF 之間有 gate
            WL_Q_0 = abs(ff->OutPin(0)->x() - it->first->InPin(0)->x()) +
                     abs(ff->OutPin(0)->y() - it->first->InPin(0)->y());
            double radius = (slack + dis_delay * WL_Q_0) / dis_delay;
            Rhombus *ans = new Rhombus(it->first->InPin(0)->x(), it->first->InPin(0)->y(), radius);
            multi_region.push_back(ans);
        }
    }
    return multi_region;
}
void Placement::constructFeasible(Module *ff)
{
    Rhombus *in = findInputRegion(ff);
    vector<Rhombus *> out = findOutputRegion(ff);
    if (out.empty())
    {

        return;
    }
    out.push_back(in); // in_and_out
    double Feas_x1 = 0, Feas_y1 = 0, Feas_x2 = 0, Feas_y2 = 0;
    if (overlap_ornot(out, Feas_x1, Feas_x2, Feas_y1, Feas_y2) == 1)
    { // if return is true then Feas_x1, Feas_x2, Feas_y1, Feas_y2 have correct value
        Rectangle *buff = new Rectangle(Feas_x1, Feas_y1, Feas_x2, Feas_y2);
        ff->setFeasibleRegion(buff); // feasibleRegion still be retangleable
    }
    else
    {
        // cout << ff->name() << " feasibleRegion not found\n"
        //      << endl;
    }
}

void Placement::netListGraph()
{
    for (int i = 0; i < this->_dataBase->getNumFF(); i++)
    {
        // clang-format off
        deque<pair<Pin *, vector<int> > > que;
        // clang-format on
        for (int j = 0; j < this->_dataBase->ff(i)->numOutPins(); j++)
        {
            vector<int> a;
            a.push_back(this->_dataBase->ff(i)->No);
            que.push_back(make_pair(this->_dataBase->ff(i)->OutPin(j), a));
        }
        while (!que.empty())
        {
            Module *moduleptr = que.front().first->module();
            if (moduleptr->isFF() && moduleptr->name() != this->_dataBase->ff(i)->name())
            {
                // que.front().second.size()-1 is moduleptr BenZen
                // que.front().second.size()-2 is prelevel Fatboy of moduleptr
                Module *PreModule = this->getDatabase()->getIntModule(que.front().second[que.front().second.size() - 2]);
                if (PreModule->isFF())
                {
                    Module *temp;
                    temp = NULL;
                    this->_dataBase->ff(i)->_outputFF.insert(make_pair(temp, moduleptr));
                }
                if (!PreModule->isFF())
                {
                    this->_dataBase->ff(i)->_outputFF.insert(make_pair(PreModule, moduleptr));
                }
                que.front().second.clear();
                // shirink_to_fit 無法使用
                // que.front().second.shrink_to_fit();
                vector<int>(que.front().second).swap(que.front().second);

                que.pop_front();
                continue;
            }
            if (moduleptr == NULL)
            {
                cout << "BUG" << endl;
                exit(0);
            }
            for (int j = 0; j < moduleptr->numOutPins(); j++)
            {
                for (int z = 0; z < moduleptr->OutPin(j)->net()->numPins(); z++)
                {
                    bool IO_Design = 0;
                    if (moduleptr->OutPin(j)->net()->pin(z)->module() == NULL)
                    {
                        if (this->_dataBase->IODesign.find(moduleptr->OutPin(j)->net()->pin(z)->name()) != this->_dataBase->IODesign.end())
                        {
                            continue;
                        }
                        else
                        {
                            exit(0);
                        }
                    }
                    // auto it = std::find(que.front().second.begin(), que.front().second.end(), moduleptr->OutPin(j)->net()->pin(z)->module()->No);
                    vector<int>::iterator it = std::find(que.front().second.begin(), que.front().second.end(), moduleptr->OutPin(j)->net()->pin(z)->module()->No);
                    if (z == moduleptr->OutPin(j)->net()->getOutIdx() || it != que.front().second.end())
                    {
                        if (it != que.front().second.end() && moduleptr->OutPin(j)->net()->pin(z)->module()->name() == "C101355")
                        {
                        }
                    }
                    else
                    {
                        que.push_back(make_pair(moduleptr->OutPin(j)->net()->pin(z), que.front().second));
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
            // que.front().second.shrink_to_fit();
            vector<int>(que.front().second).swap(que.front().second);

            que.pop_front();
        }
        if (i % 1000 == 0)
        {
            // cout << i << endl;
            // cout << this->getDatabase()->ff(i)->name() << endl;
            // clang-format off
            for (set<std::pair<Module *, Module *> >::iterator it = this->getDatabase()->ff(i)->_outputFF.begin(); it != this->getDatabase()->ff(i)->_outputFF.end(); ++it)
            { // clang-format on
                if (it->first == NULL)
                {
                    // cout << "Gate: NULL   FF:" << it->second->name() << endl;
                }
                else
                {
                    // cout << "Gate: " << it->first->name() << "   FF:" << it->second->name() << endl;
                }
            }
        }

    } // Inside is gragh Funtion
}

void Placement::debankAllFF()
{
    unsigned initFFnum = _dataBase->getNumFF();
    // cout << "initFFnum: " << initFFnum << endl;
    // cout << "module num " << _dataBase->getNumModules() << endl;
    for (size_t i = 0; i < initFFnum; i++)
    {
        if (_dataBase->ff(i)->cellType()->numBit() > 1)
        {
            string Dname = _dataBase->ff(i)->name();
            // cout << "ffname: " << Dname << endl;
            debankFFto1bit(Dname);
        }
    }
    return;
}

void Placement::debankFFto1bit(string ffname)
{
    cout << "debank" << endl;
    // TODO: Each FF position after debanking
    int celltypeID = 0;
    Module *target = _dataBase->getModuleByName(ffname);
    int ffbit = target->cellType()->getnumBit();
    vector<Module *> newfflist;
    newfflist.clear();
    string pinName;
    // TODO:radius
    // clang-format off
    vector<pair<double, double> > newPos;
    // clang-format on
    newPos.clear();
    for (size_t i = 0; i < ffbit; i++)
    {
        newPos.push_back(make_pair(target->x(), target->y()));
        newfflist.push_back(new Module());
        newfflist[i]->setCellType(_dataBase->getBestCelltype(1));
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
    // naming =================================================================
    string nname = _dataBase->module(_dataBase->getNumModules() - 1)->name();
    string letters;
    string numbers;
    for (string::size_type i = 0; i < nname.size(); ++i)
    {
        char c = nname[i];
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            numbers += c;
        }
        else
        {
            letters += c;
        }
    }
    int num = my_stoi(numbers);
    for (size_t i = 0; i < ffbit - 1; i++)
    {
        num++;
        nname = letters + my_itos(num);
        newfflist[i]->setName(nname);
    }
    // naming =================================================================
    newfflist[ffbit - 1]->setInPin(newfflist[ffbit - 1]->cellType()->clkPinIdx(), target->InPin(target->cellType()->clkPinIdx()));
    newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->setModulePtr(newfflist[ffbit - 1]);
    newfflist[ffbit - 1]->setName(target->name());
    for (size_t i = 0; i < ffbit - 1; i++)
    {
        Pin *newCLK = new Pin();
        string n = "CLK";
        newCLK->setPinName(n);
        newCLK->setOffset(newfflist[i]->cellType()->pinOffsetX(2),
                          newfflist[i]->cellType()->pinOffsetY(2));
        newCLK->setPosition(newPos[i].first + newCLK->x(), newPos[i].second + newCLK->y());
        newCLK->setModulePtr(newfflist[i]);
        newfflist[i]->setInPin(newfflist[i]->cellType()->clkPinIdx(), newCLK);
        // connecting pin to net
        newCLK->setNetPtr(newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->net());
        newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->net()->addPin(newCLK);
        History *newH = new History();
        newH->setNewPin(newCLK);
        newH->setOldPinName(n);
        newH->setOldModuleName(ffname);
        newCLK->setHistory(newH);
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
    free(target->getFeasibleRegion());
    free(target);
    for (size_t i = 0; i < ffbit - 1; i++)
    {
        _dataBase->addFF(newfflist[i]);
        cout << "module name : " << newfflist[i]->name() << endl;
        _dataBase->addModule(newfflist[i]);
    }
    return;
}
bool isStrictSubset(const set<Module *> &a, const set<Module *> &b)
{
    return includes(b.begin(), b.end(), a.begin(), a.end());
}

bool compareFirst(const Edge &a,
                  const Edge &b)
{
    return a.y > b.y;
}
// clang-format off
set<set<Module *> > Placement::calMaxClique(Net * targetNet)
{ // clang-format on
    // input : clk net id
    // output : maximal clique(without proper subset)
    // clang-format off
    set<set<Module *> > maxClique;
    vector<set<Module *> > tempClique; // size == 3 , for checking greater than above and below
    maxClique.clear();
    // clang-format on
    // Net *targetNet = _dataBase->net(clkidx);
    if (targetNet->clkFlag() == false)
    {
        cout << "error: input isn't a clk" << endl;
        return maxClique;
    }
    ModuleList targetFFs;
    vector<double> strip; // start pos of every part of strip
    vector<Edge> edges;   // Module, y , start x , end x
    edges.clear();
    edges.clear();
    strip.clear();
    targetFFs.clear();
    for (size_t i = 0; i < targetNet->numPins(); i++)
    {
        if (targetNet->pin(i)->module() == NULL)
        {
            continue;
        }
        if (targetNet->pin(i)->module()->getFeasibleRegion() != NULL)
        {
            targetFFs.push_back(targetNet->pin(i)->module());
            strip.push_back(targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->left());
            // cout << "left : " << targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->left() << endl;
            strip.push_back(targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->right());
            // cout << "right : " << targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->right() << endl;
            Edge ee;
            ee.ff = targetFFs[targetFFs.size() - 1];
            ee.y = targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->top();
            ee.isIN = true;
            ee.startx = targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->left();
            ee.endx = targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->right();
            if (ee.endx - ee.startx != 0)
            {
                edges.push_back(ee);
            }
            Edge ee1;
            ee1.ff = targetFFs[targetFFs.size() - 1];
            ee1.y = targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->bottom();
            ee1.isIN = false;
            ee1.startx = targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->left();
            ee1.endx = targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->right();
            if (ee1.endx - ee1.startx != 0)
            {
                edges.push_back(ee1);
            }
        }
    }
    sort(strip.begin(), strip.end());
    strip.erase(unique(strip.begin(), strip.end()), strip.end());
    sort(edges.begin(), edges.end(), compareFirst);
    cout << edges.size() << endl;
    int counter = 0;
    // TODO: currently traverse every edge per strip, maybe a better O() way to implement
    for (size_t i = 0; i < strip.size() - 1; i++)
    {
        counter = 0;
        tempClique.clear();
        tempClique.resize(3);
        for (size_t j = 0; j < edges.size(); j++)
        {
            // sweep line
            if ((edges[j].startx == strip[i] && edges[j].endx >= strip[i + 1]) ||
                (edges[j].startx <= strip[i] && edges[j].endx == strip[i + 1]) ||
                (edges[j].startx < strip[i] && edges[j].endx > strip[i + 1]) ||
                (edges[j].startx == strip[i] && edges[j].endx == strip[i + 1]))
            {
                if (counter == 0)
                { // check if Module is in the set
                    counter++;
                    if (edges[j].isIN == false)
                    {
                        cout << "error1" << endl;
                        cout << edges[j].startx << "  " << edges[j].endx << endl;
                    }
                    else
                    {
                        tempClique[0].insert(edges[j].ff);
                    }
                }
                else if (counter == 1)
                {
                    counter++;
                    if (edges[j].isIN == false)
                    {
                        tempClique[1] = tempClique[0];
                        tempClique[1].erase(edges[j].ff);
                        if (tempClique[1].size() > 0)
                        {
                            cout << "error2" << endl;
                        }
                    }
                }
                else if (counter == 2)
                {
                    counter++;
                    tempClique[2] = tempClique[1];
                    if (edges[j].isIN == false)
                    {
                        tempClique[2].erase(edges[j].ff);
                    }
                    else
                    {
                        tempClique[2].insert(edges[j].ff);
                    }
                }
                else
                {
                    counter++;
                    // move tempClique
                    if (tempClique[1].size() > tempClique[0].size() &&
                        tempClique[1].size() > tempClique[2].size() &&
                        tempClique[1].size() > 1)
                    {
                        maxClique.insert(tempClique[1]);
                    }
                    tempClique[0] = tempClique[1];
                    tempClique[1] = tempClique[2];
                    if (edges[j].isIN == false)
                    {
                        tempClique[2].erase(edges[j].ff);
                    }
                    else
                    {
                        tempClique[2].insert(edges[j].ff);
                    }
                }
            }
            else
            {
                continue;
            }
        }
    }
    // TODO: remove proper subset
    // clang-format off
    vector<set<Module *> > toRemove;
    
    for (set<set<Module*> >::iterator it1 = maxClique.begin(); it1 != maxClique.end(); ++it1)
    {
        for (set<set<Module*> >::iterator it2 = maxClique.begin(); it2 != maxClique.end(); ++it2)
        {
            if (it1 != it2 && isStrictSubset(*it1, *it2))
            {
                toRemove.push_back(*it1);
                break;
            }
        }
    }
    for (vector<set<Module*> >::const_iterator it = toRemove.begin(); it != toRemove.end(); ++it)
    {
        maxClique.erase(*it);
    }
    // clang-format on
    return maxClique;
}
