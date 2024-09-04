#include "Placement.h"
// #include <mutex> // threads header file but cannot use in C++98
// #include <thread>
#include <cctype>
#include <cmath>
#include <cstdlib>
// #include <ctime>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <set>
#include <math.h>
#include <set>
#include <cfloat>
#include <queue>
#include <utility>

#include <stack>
using namespace std;

#define modWidThres 1
#define leafthresold 0.75 // TODO: can be changed
// #define __DBL_MAX__ 1.7976931348623158e+308 /* max value */
struct Edges // for calMaxClique
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
// clang-format off
pair<pair<double, double>, pair<double, double> > overlapRegion(const vector<Rectangle *> rectangles)
{ // clang-format on
    if (rectangles.empty())
    {
        return make_pair(make_pair(-1, -1), make_pair(-1, -1));
    }
    double left = rectangles[0]->left();
    double bottom = rectangles[0]->bottom();
    double right = rectangles[0]->right();
    double top = rectangles[0]->top();

    for (size_t i = 1; i < rectangles.size(); ++i)
    {
        left = max(left, rectangles[i]->left());
        bottom = max(bottom, rectangles[i]->bottom());
        right = min(right, rectangles[i]->right());
        top = min(top, rectangles[i]->top());
    }

    if (left >= right || bottom >= top)
    {
        return make_pair(make_pair(-1, -1), make_pair(-1, -1));
    }
    return make_pair(make_pair(left, bottom), make_pair(right, top));
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
            (*it) = NULL;
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
    if (num == 0)
    {
        return "0";
    }
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
    // // TODO: need TO clear net clk pin
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
// This Func can only merge when the set exact number
void Placement::mergeMulti1bitFF(set<Module *> ffs)
{ // input ffs size must be exactly same as new FF bit size /////////
    // TODO: when merging, need to
    if (ffs.size() > _dataBase->getmaxLibBit())
    {
        cout << "Error: Cannot merge multi-bit FFs. The number of FFs is greater than the maximum library bit size." << endl;
        cout << ffs.size() << endl;
        return;
    }
    _maxClique.erase(ffs);
    if (ffs.size() == 1)
    {
        return;
    }
    vector<Module *> ffsV;
    ffsV.assign(ffs.begin(), ffs.end());
    // cout << "merging ";
    // for (size_t i = 0; i < ffsV.size(); i++)
    // {
    //     cout << ffsV[i]->name() << "  ";
    // }
    pair<double, double> ffPos;
    // remove the freed Module from other Cliques////////////////////////////////////
    for (size_t i = 0; i < ffsV.size(); i++)
    {
        // clang-format off
        // TODO: during for loop , but insert new element , will be wrong
        set<set<Module*> > tempClique;
        tempClique.clear();
        for (set<set<Module*> >::iterator it = _maxClique.begin(); it != _maxClique.end(); ++it)
        { // clang-format on
            const set<Module *> &mySet = *it;
            set<Module *> innerSet = mySet;
            if (mySet == ffs)
            {
                continue;
            }
            innerSet.erase(ffsV[i]);
            if (innerSet.size() != 0)
            {
                tempClique.insert(innerSet);
            }
        }
        _maxClique = tempClique;
    }
    // temp position/////////////////////////////////////////////////////////////////
    // TODO: new position
    vector<Rectangle *> feasibleRegV;
    feasibleRegV.clear();
    for (size_t i = 0; i < ffsV.size(); ++i)
    {
        feasibleRegV.push_back(ffsV[i]->getFeasibleRegion());
    }
    // clang-format off
    pair<pair<double, double>, pair<double, double> > olArea = overlapRegion(feasibleRegV);
    // clang-format on
    ffPos.first = (olArea.first.first + olArea.second.first) / 2;
    ffPos.second = (olArea.second.second + olArea.first.second) / 2;
    /////////////////////////////////////////////////////////////////////////////////
    // naming////////////////////////////////////////////////////////////////////////
    string newFFname = _dataBase->module(_dataBase->getNumModules() - 1)->name();
    string letters;
    string numbers;
    for (string::size_type i = 0; i < newFFname.size(); ++i)
    {
        char c = newFFname[i];
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
    num++;
    newFFname = letters + my_itos(num);
    Module *newMff = new Module(newFFname, _dataBase->getBestCelltype(ffsV.size()), ffPos.first, ffPos.second);
    newMff->setIsFixed(false);
    newMff->clearPins();
    newMff->setPinsize(newMff->cellType()->getnumBit() * 2 + 1);
    // radius and feasible region will be empty for the new FF
    // trasplant pins to new ff ///////////////////////////////////////////////////////////////
    for (size_t i = 0; i < ffsV.size(); i++)
    {
        string tempName = "D";
        tempName = tempName + my_itos(i);
        newMff->setInPin(i, ffsV[i]->InPin(0));
        newMff->InPin(i)->setPinName(tempName);
        newMff->InPin(i)->setModulePtr(newMff);
        tempName = "Q";
        tempName = tempName + my_itos(i);
        newMff->setOutPin(i, ffsV[i]->OutPin(0));
        newMff->OutPin(i)->setPinName(tempName);
        newMff->OutPin(i)->setModulePtr(newMff);
    }
    // only first FF CLK pin will remain //////////////////////////////////////////////////////////////
    newMff->setInPin(newMff->cellType()->clkPinIdx(), ffsV[0]->InPin(ffsV[0]->cellType()->clkPinIdx()));
    if (ffsV[0]->InPin(1)->name() != "CLK")
    {
        cout << "error: target pin isn't CLK" << endl;
    }
    newMff->InPin(newMff->cellType()->clkPinIdx())->setModulePtr(newMff);
    // update Module in Pin
    // update pin position and x,y offset
    for (size_t i = 0; i < newMff->totnumPins(); i++)
    {
        newMff->pin(i)->setOffset(newMff->cellType()->pinOffsetX(i), newMff->cellType()->pinOffsetY(i));
        newMff->pin(i)->setPosition(newMff->x() + newMff->pin(i)->xOffset(), newMff->y() + newMff->pin(i)->yOffset());
    }
    for (size_t i = 1; i < ffsV.size(); i++)
    {
        Pin *targetclk = ffsV[i]->InPin(1);
        if (targetclk->name() != "CLK")
        {
            cout << "error: target pin isn't CLK" << endl;
        }
        else
        {
            if (targetclk->history() != NULL)
            {
                delete targetclk->history();
                targetclk->setHistory(NULL);
            }
            for (size_t j = 0; j < _dataBase->getNumPins(); j++)
            {
                if (_dataBase->pin(j) == targetclk)
                {
                    _dataBase->erasePin(j);
                    break;
                }
            }
            for (size_t j = 0; j < targetclk->net()->numPins(); j++)
            {
                if (targetclk->net()->pin(j) == targetclk)
                {
                    targetclk->net()->erasePin(j);
                    break;
                }
            }
            delete targetclk;
            targetclk = NULL;
        }
    }
    // clear FFs with no neighbor out of graph(_nodes)/////////////////////////////////
    // free m1 and m2
    // cout << "new FF name " << newMff->name() << " " << newMff->cellType()->getName() << endl;
    for (size_t i = 0; i < ffsV.size(); i++)
    {
        _dataBase->delIntModule(ffsV[i]->No);
        DelmoduleAss(ffsV[i]);
        for (size_t j = 0; j < _dataBase->getNumModules(); j++)
        {
            if (_dataBase->module(j) == ffsV[i])
            {
                _dataBase->eraseModule(j);
                break;
            }
        }
        for (size_t j = 0; j < _dataBase->getNumFF(); j++)
        {
            if (_dataBase->ff(j) == ffsV[i])
            {
                _dataBase->eraseFF(j);
                break;
            }
        }
        delete ffsV[i];
        ffsV[i] = NULL;
    }
    _dataBase->addModule(newMff);
    _dataBase->addFF(newMff);
    _dataBase->addIntModule(num, newMff);
    newMff->No = num;
    newMff->setCenterPosition(ffPos.first, ffPos.second);
    feasibleRegV.clear();
    ffsV.clear();
    ffs.clear();
    _moduleNeedAss.push_back(newMff);
    // row assignment//////////////////////////////////////////////
    // double upperY, lowerY;
    // double targetPosY = 0;
    // unsigned rowID = 0;
    // for (size_t i = 0; i < _dataBase->getNumRows() - 1; i++)
    // {
    //     lowerY = _dataBase->row(i)->y();
    //     upperY = _dataBase->row(i + 1)->y();
    //     if (newMff->y() >= lowerY && newMff->y() < upperY)
    //     {
    //         rowID = i;
    //         break;
    //     }
    // }
    // if ((newMff->y() - lowerY) > (upperY - newMff->y()))
    // {
    //     targetPosY = upperY;
    //     rowID++;
    // }
    // else
    // {
    //     targetPosY = lowerY;
    // }
    // while (_dataBase->row(rowID)->y() + newMff->height() > _dataBase->getBoundaryTop())
    // {
    //     rowID--;
    // }

    // row assignment //////////////////////////////////////////////
    // upperY = 0;
    // lowerY = 0;
    // double targetDobX = 0;
    // unsigned targetPosX = 0;
    // if (newMff->x() < _dataBase->row(rowID)->x())
    // {
    //     newMff->setPosition(_dataBase->row(rowID)->x(), targetPosY);
    // }
    // else
    // {
    //     targetDobX = (newMff->x() - _dataBase->row(rowID)->x()) / _dataBase->row(rowID)->width();
    //     targetPosX = static_cast<unsigned>(round(targetDobX));
    //     newMff->setPosition(_dataBase->row(rowID)->x() + targetPosX * _dataBase->row(rowID)->width(), targetPosY);
    // }
    // // check boundary
    // while (newMff->x() + newMff->width() > _dataBase->getBoundaryRight())
    // {
    //     targetPosX--;
    //     newMff->setPosition(_dataBase->row(rowID)->x() + targetPosX * _dataBase->row(rowID)->width(), targetPosY);
    // }
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
        delete _nodes[i];
        _nodes[i] = NULL;
    }
    _nodes.clear();
    return;
}

NodeList Placement::findMST()
{
    // // graph should be in _nodes////////////////////////////////
    // // clang-format off
    // vector<pair<Node *, pair<double, Node *> > > qheap; // <Node,key,predecessor>
    // qheap.resize(_nodes.size());
    // for (size_t i = 0; i < _nodes.size(); i++)
    // {
    //     _nodes[i]->setNodeidxheap(i);
    //     qheap[i].first = _nodes[i];
    //     qheap[i].second.first = __DBL_MAX__;
    //     qheap[i].second.second = NULL;
    // }
    // qheap[0].second.first = 0;
    // vector<pair<Node *, pair<double, Node *> > > mstHeap;
    NodeList mst;
    // while (qheap.size() > 0)
    // {
    //     // pair<Node *, pair<double, Node *> > min = extractMinMST(qheap);
    //     // min.first->setNodeidxheap(-1);
    //     // map<string, pair<Node *, double> > neighbor = min.first->getneighbormap(); // previous node's neighbor
    //     // for (map<string, pair<Node*, double> >::iterator it = neighbor.begin(); it != neighbor.end(); ++it)
    //     //{
    //     //     if (pair.second.first->getNodeidxheap() != -1)
    //     //     {
    //     //         if (pair.second.second < qheap[pair.second.first->getNodeidxheap()].second.first)
    //     //         {
    //     //             qheap[pair.second.first->getNodeidxheap()].second.second = min.first; // set predecessor
    //     //             DecreaseKeyMST(qheap, pair.second.first->getNodeidxheap(), pair.second.second);
    //     //         }
    //     //     }
    //     pair<Node *, pair<double, Node *> > min = extractMinMST(qheap);
    //     min.first->setNodeidxheap(-1);
    //     map<string, pair<Node *, double> > neighbor = min.first->getneighbormap(); // previous node's neighbor
    //     for (map<string, pair<Node *, double> >::iterator it = neighbor.begin(); it != neighbor.end(); ++it)
    //     {
    //         if (it->second.first->getNodeidxheap() != -1)
    //         {
    //             if (it->second.second < qheap[it->second.first->getNodeidxheap()].second.first)
    //             {
    //                 qheap[it->second.first->getNodeidxheap()].second.second = min.first; // set predecessor
    //                 DecreaseKeyMST(qheap, it->second.first->getNodeidxheap(), it->second.second);
    //             }
    //         }
    //     }
    //     mstHeap.push_back(min);
    // } // clang-format on
    // for (size_t i = 0; i < mstHeap.size(); ++i)
    // {
    //     mst.push_back(new Node(mstHeap[i].first->getFFinNode()));
    //     mst[i]->setNodeidxheap(i);
    //     mstHeap[i].first->setNodeidxheap(i);
    //     mst[i]->setisleaf(false);
    // }
    // for (size_t i = mstHeap.size() - 1; i >= 1; --i)
    // {
    //     mst[i]->addNeighborPair(make_pair(mst[mstHeap[i].second.second->getNodeidxheap()], mstHeap[i].second.first));
    //     unsigned id = mstHeap[i].second.second->getNodeidxheap();
    //     mst[id]->addNeighborPair(make_pair(mst[i], mstHeap[i].second.first));
    // }
    // // print MST ///////////////////////////////////////////////
    // // for (size_t i = 0; i < mst.size(); i++)
    // // {
    // //     cout << "Node " << mst[i]->getFFinNode()->name() << " has neighbor: ";
    // //     map<string, pair<Node *, double> > neighbor = mst[i]->getneighbormap();
    // //     for (const auto &pair : neighbor)
    // //     {
    // //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    // //     }
    // //     cout << endl;
    // // }
    // // print MST ///////////////////////////////////////////////
    // mstHeap.clear();
    // qheap.clear();
    // return mst;
    // //  print the heap//////////////////////////////////////////
    // // for (size_t i = 0; i < qheap.size(); i++)
    // // {
    // //     cout << "Node " << qheap[i].first->getFFinNode()->name() << " has neighbor: ";
    // //     cout << " " << qheap[i].first->getNodeidxheap() << " " << endl;
    // //     cout << "key : " << qheap[i].second.first << endl;
    // //     map<string, pair<Node *, double> > neighbor = qheap[i].first->getneighbormap();
    // //     for (const auto &pair : neighbor)
    // //     {
    // //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    // //     }
    // //     cout << endl;
    // // }
    // // print the heap//////////////////////////////////////////
    return mst;
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

bool Placement::overlap_ornot(vector<Rhombus *> &input_rhombus, double &leftBound, double &rightBound, double &botBound, double &topBound)
{
    if (input_rhombus.empty())
        return false;

    leftBound = _dataBase->row(0)->x();
    rightBound = _dataBase->row(0)->x() + _dataBase->row(0)->numSites() * _dataBase->row(0)->width();
    botBound = _dataBase->row(0)->y();
    topBound = _dataBase->row(_dataBase->getNumRows() - 1)->y();

    for (vector<Rhombus *>::iterator it = input_rhombus.begin(); it != input_rhombus.end(); ++it)
    {
        (*it)->RotatePeak(45);
    }

    for (vector<Rhombus *>::iterator it = input_rhombus.begin(); it != input_rhombus.end(); ++it)
    {
        leftBound = std::max(leftBound, (*it)->peak(3).x);
        rightBound = std::min(rightBound, (*it)->peak(1).x);
        botBound = std::max(botBound, (*it)->peak(3).y);
        topBound = std::min(topBound, (*it)->peak(1).y);
    }

    for (vector<Rhombus *>::iterator it = input_rhombus.begin(); it != input_rhombus.end(); ++it)
    {
        (*it)->RotatePeak(-45);
    }

    if (leftBound < _dataBase->row(0)->x())
        leftBound = _dataBase->row(0)->x();
    if (rightBound > _dataBase->row(0)->x() + _dataBase->row(0)->numSites() * _dataBase->row(0)->width())
        rightBound = _dataBase->row(0)->x() + _dataBase->row(0)->numSites() * _dataBase->row(0)->width();
    if (botBound < _dataBase->row(0)->y())
        botBound = _dataBase->row(0)->y();
    if (topBound > _dataBase->row(_dataBase->getNumRows() - 1)->y())
        topBound > _dataBase->row(_dataBase->getNumRows() - 1)->y();

    if ((leftBound >= rightBound) || (botBound >= topBound))
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
    _nodes.clear();
    int num_FF = _dataBase->getNumFF();
    for (int i = 0; i < num_FF; ++i)
    {
        // Create a new node for the current FF.
        Node *n = new Node;
        n->setFFinNode(_dataBase->ff(i));
        _name2Node[n->getFFinNode()->name()] = n;
        _nodes.push_back(n);

        // Construct feasible region for current FF
        constructFeasible(_dataBase->ff(i));
    }
    for (int i = 0; i < num_FF; ++i)
    {
        for (int j = i + 1; j < num_FF; ++j)
        {
            if (_dataBase->ff(i)->getFeasibleRegion() == NULL)
                continue;
            if (_dataBase->ff(j)->getFeasibleRegion() == NULL)
                continue;
            if (feasibleRegion_overlap(_dataBase->ff(i)->getFeasibleRegion(), _dataBase->ff(j)->getFeasibleRegion()))
            {
                _name2Node[_dataBase->ff(i)->name()]->addNeighborPair({_name2Node[_dataBase->ff(j)->name()], 0}); // without cost
                _name2Node[_dataBase->ff(j)->name()]->addNeighborPair({_name2Node[_dataBase->ff(i)->name()], 0}); // two directions
            }
        }
    }
}

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
    cost += lambda * _dataBase->updateBinUtil();

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
        if (abs(Feas_x1) < pow(10, -10))
            Feas_x1 = 0;
        if (abs(Feas_y1) < pow(10, -10))
            Feas_y1 = 0;
        if (abs(Feas_x2) < pow(10, -10))
            Feas_x2 = 0;
        if (abs(Feas_y2) < pow(10, -10))
            Feas_y2 = 0;

        ff->setFeasibleRegion(buff); // feasibleRegion still be retangleable
    }
    else
    {
        ff->setFeasibleRegion(NULL);
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
            // cout << __LINE__ <<endl;
        }
        while (!que.empty())
        {
            // // cout << __LINE__ << endl;
            Module *moduleptr = que.front().first->module();
            // // cout << moduleptr->name() << endl;
            // // cout << "outpin: " << moduleptr->numOutPins() << endl;
            // // cout << moduleptr->cellType()->pinNum() << endl;
            // // cout << moduleptr->cellType();

            if (moduleptr->isFF() && moduleptr->name() != this->_dataBase->ff(i)->name())
            {
                // cout << __LINE__ << endl;
                // cout << "Bit num: "<< moduleptr->cellType()->getnumBit() << endl;
                //  for(int i = 0;i < moduleptr->totnumPins(); i++)
                //  {
                //      //cout << "pins: " << i << endl;
                //  }
                //  cout << "net: " << moduleptr->pin(0)->net()->name() << endl;
                //  cout << "net: " << moduleptr->pin(0) << endl;
                //  cout << "net pin : " << moduleptr->pin(0)->net()->pin(1) << endl;
                //  cout << "net: " << moduleptr->pin(1)->net()->name() << endl;
                //  cout << "net: " << moduleptr->pin(2)->net()->name() << endl;

                // que.front().second.size()-1 is moduleptr
                // que.front().second.size()-2 is prelevel of moduleptr
                Module *PreModule = this->getDatabase()->getIntModule(que.front().second[que.front().second.size() - 2]);
                cout << this->getDatabase()->getIntModule(que.front().second[que.front().second.size() - 1])->name() << endl;
                cout << this->getDatabase()->getIntModule(que.front().second[que.front().second.size() - 1])->InPin(0)->net()->name() << endl;
                // cout << __LINE__ << endl;
                //  if(PreModule == NULL)
                //  {
                //      cout << "NULL ptr!!" <<endl;
                //  }
                if (PreModule->isFF())
                {
                    // cout << __LINE__ << endl;
                    Module *temp;
                    temp = NULL;
                    this->_dataBase->ff(i)->_outputFF.insert(make_pair(temp, moduleptr));
                }
                if (!PreModule->isFF())
                {
                    // cout << __LINE__ << endl;
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
                    // cout << __LINE__ << endl;
                    bool IO_Design = 0;
                    if (moduleptr->OutPin(j)->net()->pin(z)->module() == NULL)
                    {
                        if (this->_dataBase->IODesign.find(moduleptr->OutPin(j)->net()->pin(z)->name()) != this->_dataBase->IODesign.end())
                        {
                            // cout << __LINE__ << endl;
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
    ModuleList tempList = _dataBase->getWholeffList();
    for (size_t i = 0; i < tempList.size(); i++)
    {
        if (tempList[i]->cellType()->numBit() > 1)
        {
            string Dname = tempList[i]->name();
            debankFFto1bit(Dname);
            tempList[i] = NULL;
        }
    }
    tempList.clear();
    return;
}

void Placement::debankFFto1bit(string ffname)
{
    // cout << "debank  " << ffname << endl;
    // TODO: Each FF position after debanking
    int celltypeID = 0;
    Module *target = _dataBase->getModuleByName(ffname);
    int ffbit = target->cellType()->getnumBit();
    vector<Module *> newfflist;
    newfflist.clear();
    string pinName = "";
    // TODO:radius
    // clang-format off
    vector<pair<double, double> > newPos;
    // clang-format on
    newPos.clear();
    for (size_t i = 0; i < ffbit; i++)
    {
        newPos.push_back(make_pair(target->centerX(), target->centerY()));
        Module *nff = new Module();
        if (nff == NULL)
        {
            cerr << "Memory allocation failed" << endl;
        }
        newfflist.push_back(nff);
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
    string letters = "";
    string numbers = "";
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
    for (size_t i = 0; i < ffbit; i++)
    {
        num++;
        nname = letters + my_itos(num);
        newfflist[i]->setName(nname);
        _dataBase->addIntModule(num, newfflist[i]);
        newfflist[i]->No = num;
    }
    // naming =================================================================
    newfflist[ffbit - 1]->setInPin(newfflist[ffbit - 1]->cellType()->clkPinIdx(), target->InPin(target->cellType()->clkPinIdx()));
    newfflist[ffbit - 1]->InPin(newfflist[ffbit - 1]->cellType()->clkPinIdx())->setModulePtr(newfflist[ffbit - 1]);
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
    // postion //////////////////////////////////////////
    for (size_t i = 0; i < ffbit; i++)
    {
        newfflist[i]->setCenterPosition(newPos[i].first, newPos[i].second);
        _moduleNeedAss.push_back(newfflist[i]);
    }
    // double upperY, lowerY;
    // double targetPosY = 0;
    // unsigned rowID = 0;
    // for (size_t i = 0; i < _dataBase->getNumRows() - 1; i++)
    // {
    //     lowerY = _dataBase->row(i)->y();
    //     upperY = _dataBase->row(i + 1)->y();
    //     if (newfflist[0]->y() >= lowerY && newfflist[0]->y() < upperY)
    //     {
    //         rowID = i;
    //         break;
    //     }
    // }
    // if ((newfflist[0]->y() - lowerY) > (upperY - newfflist[0]->y()))
    // {
    //     targetPosY = upperY;
    //     rowID++;
    // }
    // else
    // {
    //     targetPosY = lowerY;
    // }
    // while (_dataBase->row(rowID)->y() + newfflist[0]->height() > _dataBase->getBoundaryTop())
    // {
    //     rowID--;
    // }
    // upperY = 0;
    // lowerY = 0;
    // double targetDobX = 0;
    // unsigned targetPosX = 0;
    // if (newfflist[0]->x() < _dataBase->row(rowID)->x())
    // {
    //     for (size_t i = 0; i < newfflist.size(); i++)
    //     {
    //         newfflist[i]->setPosition(_dataBase->row(rowID)->x(), targetPosY);
    //     }
    // }
    // else
    // {
    //     targetDobX = (newfflist[0]->x() - _dataBase->row(rowID)->x()) / _dataBase->row(rowID)->width();
    //     targetPosX = static_cast<unsigned>(round(targetDobX));
    //     for (size_t i = 0; i < newfflist.size(); i++)
    //     {
    //         newfflist[0]->setPosition(_dataBase->row(rowID)->x() + targetPosX * _dataBase->row(rowID)->width(), targetPosY);
    //     }
    // }
    // while (newfflist[0]->x() + newfflist[0]->width() > _dataBase->getBoundaryRight())
    // {
    //     targetPosX--;
    //     newfflist[0]->setPosition(_dataBase->row(rowID)->x() + targetPosX * _dataBase->row(rowID)->width(), targetPosY);
    // }
    ////////////////////////////////////////////////
    for (size_t i = 0; i < _dataBase->getNumModules(); i++)
    {
        if (_dataBase->module(i) == target)
        {
            _dataBase->eraseModule(i);
            break;
        }
    }
    for (size_t i = 0; i < _dataBase->getNumFF(); i++)
    {
        if (_dataBase->ff(i) == target)
        {
            _dataBase->eraseFF(i);
            break;
        }
    }
    _dataBase->delIntModule(target->No);
    delete target;
    target = NULL;
    for (size_t i = 0; i < ffbit; i++)
    {
        _dataBase->addFF(newfflist[i]);
        _dataBase->addModule(newfflist[i]);
    }
    newfflist.clear();
    newPos.clear();
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
// TODO: maybe can be changed to void func
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
        if (targetNet->pin(i)->module()->cellType()->isFF() == false)
        {
            continue;
        }
        if (targetNet->pin(i)->name() != "CLK")
        {
            cout << "error: pin isn't CLK" << endl;
            continue;
        }
        if (targetNet->pin(i)->module()->cellType()->numBit() != 1)
        {
            // cout << "history : " << targetNet->pin(i)->history()->oldModuleName() << " " << targetNet->pin(i)->history()->oldPinName() << endl;
            // cout << targetNet->pin(i)->name() << endl;
            // cout << targetNet->pin(i)->module()->name() << endl;
            // cout << targetNet->pin(i)->module()->cellType()->getName() << endl;
            // cout << "bit num error" << endl;
            continue;
        }
        if (targetNet->pin(i)->module()->getFeasibleRegion() != NULL)
        {
            targetFFs.push_back(targetNet->pin(i)->module());
            unsigned targetSizeID = targetFFs.size() - 1;
            strip.push_back(targetFFs[targetSizeID]->getFeasibleRegion()->left());
            // cout << "left : " << targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->left() << endl;
            strip.push_back(targetFFs[targetSizeID]->getFeasibleRegion()->right());
            // cout << "right : " << targetFFs[targetFFs.size() - 1]->getFeasibleRegion()->right() << endl;
            Edge ee;
            ee.ff = targetFFs[targetSizeID];
            ee.y = targetFFs[targetSizeID]->getFeasibleRegion()->top();
            ee.isIN = true;
            ee.startx = targetFFs[targetSizeID]->getFeasibleRegion()->left();
            ee.endx = targetFFs[targetSizeID]->getFeasibleRegion()->right();
            if (ee.endx - ee.startx != 0)
            {
                edges.push_back(ee);
            }
            Edge ee1;
            ee1.ff = targetFFs[targetSizeID];
            ee1.y = targetFFs[targetSizeID]->getFeasibleRegion()->bottom();
            ee1.isIN = false;
            ee1.startx = targetFFs[targetSizeID]->getFeasibleRegion()->left();
            ee1.endx = targetFFs[targetSizeID]->getFeasibleRegion()->right();
            if (ee1.endx - ee1.startx != 0)
            {
                edges.push_back(ee1);
            }
        }
    }
    if (strip.size() == 0 || edges.size() == 0)
    {
        edges.clear();
        strip.clear();
        maxClique.clear();
        return maxClique;
    }
    sort(strip.begin(), strip.end());
    strip.erase(unique(strip.begin(), strip.end()), strip.end());
    sort(edges.begin(), edges.end(), compareFirst);
    for (size_t i = 0; i < edges.size() - 1; i++)
    {
        if (edges[i].y == edges[i + 1].y)
        {
            // cout << "asfdasdfadfas" << endl;
            // cout << edges[i].ff->name() << " " << edges[i].isIN << endl;
            // cout << edges[i + 1].ff->name() << " " << edges[i + 1].isIN << endl;
        }
    }
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
                        for (size_t k = j; k < edges.size(); k++)
                        {
                            if (edges[j].y < edges[k].y)
                            {
                                break;
                            }
                            if (edges[j].y == edges[k].y && edges[k].isIN == true)
                            {
                                Edge temp;
                                temp = edges[j];
                                edges[j] = edges[k];
                                edges[k] = temp;
                                tempClique[0].insert(edges[j].ff);
                                break;
                            }
                        }
                        if (edges[j].isIN == false)
                        {
                            cout << "error1" << endl;
                        }
                        // for (size_t k = 0; k < edges.size(); k++)
                        // {
                        //     if (edges[j].ff == edges[k].ff && edges[j])
                        //     {
                        //         /* code */
                        //     }

                        // }
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
                    else
                    {
                        tempClique[1] = tempClique[0];
                        tempClique[1].insert(edges[j].ff);
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
    if (maxClique.size() == 0)
    {
        edges.clear();
        strip.clear();
        return maxClique;
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
    for (size_t i = 0; i < toRemove.size(); i++)
    {
        maxClique.erase(toRemove[i]);
    }
    for (set<set<Module*> >::iterator it = maxClique.begin(); it != maxClique.end(); ) {
        if (it->empty()) 
        {
            set<set<Module*> >::iterator temp = it;
            ++it;
            maxClique.erase(temp);
        } 
        else {
            ++it;
        }
    }
    toRemove.clear();
    tempClique.clear();
    strip.clear();
    edges.clear();
    targetFFs.clear();
    _maxClique = maxClique;
    return maxClique;
}
set<Module *> Placement::adjustClique(Net *targetNet, set<Module *> targetClique)
{
    set<Module *> perfectClique;
    perfectClique.clear();
    unsigned cliqueSize = targetClique.size();
    unsigned s = 1;
    unsigned upperS = 0, lowerS = 0;
    // remove targetClique from _maxClique////////////////////////////////
    _maxClique.erase(targetClique);
    while (s <= _dataBase->getmaxLibBit())
    {
        if (cliqueSize > _dataBase->getmaxLibBit())
        {
            upperS = 0;
            lowerS = _dataBase->getmaxLibBit();
            break;
        }
        if (cliqueSize == s)
        {
            return targetClique;
        }
        if (cliqueSize > s && cliqueSize < (s * 2))
        {
            lowerS = s;
            upperS = s * 2;
            break;
        }
        s = s * 2;
    }
    if (lowerS == 0)
    { // error
        cout << "error : adjustClique" << endl;
        return perfectClique;
    }
    else if (upperS == 0)
    { // Clique size is larger than maximum FF lib bit size(need to discard FFs)
        // calculate overlaparea/feasibleregion
        vector<Rectangle *> feasibleRegV;
        feasibleRegV.clear();
        for (set<Module *>::iterator it = targetClique.begin(); it != targetClique.end(); ++it)
        {
            feasibleRegV.push_back((*it)->getFeasibleRegion());
        }
        // clang-format off
        pair<pair<double, double>, pair<double, double> > olArea = overlapRegion(feasibleRegV);
        // clang-format on
        double totoverlapReg = (olArea.second.first - olArea.first.first) *
                               (olArea.second.second - olArea.first.second);
        Module *fferased;
        for (size_t i = 0; i < (cliqueSize - lowerS); i++)
        {
            double minOL = DBL_MAX;
            for (set<Module *>::iterator it = targetClique.begin(); it != targetClique.end(); ++it)
            {
                double height = (*it)->getFeasibleRegion()->getHeight();
                double width = (*it)->getFeasibleRegion()->getWidth();
                if ((height * width) == 0)
                {
                    fferased = (*it);
                }
                if ((totoverlapReg / (height * width) < minOL))
                {
                    minOL = (totoverlapReg / ((*it)->getFeasibleRegion()->getHeight() * (*it)->getFeasibleRegion()->getWidth()));
                    fferased = (*it);
                }
            }
            targetClique.erase(fferased);
        }
        if (targetClique.size() != lowerS)
        {
            cout << "error: Clique isn't correctly sized " << endl;
        }
    }
    else
    { // Clique isn't perfect sized
        if ((cliqueSize - lowerS) <= (upperS - cliqueSize))
        { // in middle or lower , kick out some FF
            vector<Rectangle *> feasibleRegV;
            feasibleRegV.clear();
            for (set<Module *>::iterator it = targetClique.begin(); it != targetClique.end(); ++it)
            {
                feasibleRegV.push_back((*it)->getFeasibleRegion());
            }
            // clang-format off
            pair<pair<double, double>, pair<double, double> > olArea = overlapRegion(feasibleRegV);
            double totoverlapReg = (olArea.second.first - olArea.first.first) *
                                   (olArea.second.second - olArea.first.second);
            Module *fferased;
            double minOL = DBL_MAX;
            for (size_t i = 0; i < (cliqueSize - lowerS); i++)
            {
                for (set<Module *>::iterator it = targetClique.begin(); it != targetClique.end(); ++it)
                {
                    double height = (*it)->getFeasibleRegion()->getHeight();
                    double width = (*it)->getFeasibleRegion()->getWidth();
                    if ((height * width) == 0)
                    {
                    fferased = (*it);
                    }
                    if ((totoverlapReg / (height * width)) < minOL)
                    {
                        minOL = (totoverlapReg / (height * width));
                        fferased = (*it);
                    }
                }
                targetClique.erase(fferased);
            }
            if (targetClique.size() != lowerS)
            {
                cout << "error: Clique isn't correctly sized 1" << endl;
            }
            
        }
        else
        { // in upper,
            // TODO: Implement include some nearby FF, maybe can change into directly merge
            vector<Rectangle *> feasibleRegV;
            feasibleRegV.clear();
            vector<pair<Module *, double> > targetFFcliq;
            targetFFcliq.clear();
            // clang-format on
            for (set<Module *>::iterator it = targetClique.begin(); it != targetClique.end(); ++it)
            {
                feasibleRegV.push_back((*it)->getFeasibleRegion());
            }
            for (size_t i = 0; i < targetNet->numPins(); i++)
            {
                if (targetNet->pin(i)->module() == NULL)
                {
                    continue;
                }
                if (targetNet->pin(i)->module()->isFixed() == true)
                {
                    continue;
                }
                if (targetNet->pin(i)->name() != "CLK")
                {
                    continue;
                }
                if (targetNet->pin(i)->module()->cellType()->numBit() != 1)
                {
                    continue;
                }
                if (targetNet->pin(i)->module()->getFeasibleRegion() != NULL)
                {
                    if (targetClique.find(targetNet->pin(i)->module()) == targetClique.end())
                    {
                        targetFFcliq.push_back(make_pair(targetNet->pin(i)->module(), -1));
                    }
                }
            }
            // clang-format off
            pair<pair<double, double>, pair<double, double> > olArea = overlapRegion(feasibleRegV);
            // clang-format on
            for (size_t i = 0; i < targetFFcliq.size(); i++)
            {
                // clang-format on
                double FeasOLdisx = min(min(abs(targetFFcliq[i].first->getFeasibleRegion()->getX1() - olArea.first.first),
                                            abs(targetFFcliq[i].first->getFeasibleRegion()->getX2() - olArea.first.first)),
                                        min(abs(targetFFcliq[i].first->getFeasibleRegion()->getX1() - olArea.second.first),
                                            abs(targetFFcliq[i].first->getFeasibleRegion()->getX2() - olArea.second.first)));
                double FeasOLdisy = min(min(abs(targetFFcliq[i].first->getFeasibleRegion()->getY1() - olArea.first.second),
                                            abs(targetFFcliq[i].first->getFeasibleRegion()->getY2() - olArea.first.second)),
                                        min(abs(targetFFcliq[i].first->getFeasibleRegion()->getY1() - olArea.second.second),
                                            abs(targetFFcliq[i].first->getFeasibleRegion()->getY2() - olArea.second.second)));
                targetFFcliq[i].second = max(FeasOLdisx, FeasOLdisy);
            }
            for (size_t i = 0; i < (upperS - cliqueSize); i++)
            {
                pair<Module *, double> target;
                target = targetFFcliq[0];
                for (size_t i = 1; i < targetFFcliq.size(); i++)
                {
                    if (targetFFcliq[i].second < target.second)
                    {
                        target = targetFFcliq[i];
                    }
                }
                // clang-format off
                targetClique.insert(target.first);
                vector<pair<Module *, double> >::iterator newEnd = std::remove(targetFFcliq.begin(), targetFFcliq.end(), target);
                targetFFcliq.erase(newEnd, targetFFcliq.end());
                // clang-format on
            }

            if (targetClique.size() != upperS)
            {
                cout << "error: Clique isn't correctly sized 2" << endl;
            }
        }
    }
    _maxClique.insert(targetClique);
    return targetClique;
}
// Leagalize - DAG

bool compareModules(const Module *a, const Module *b)
{
    if (a->x() == b->x())
    {
        return a->y() < b->y();
    }
    return a->x() < b->x();
}

int partition(std::vector<Module *> &modules, int low, int high)
{
    Module *pivot = modules[high];
    int i = low - 1;

    for (int j = low; j < high; ++j)
    {
        if (compareModules(modules[j], pivot))
        {
            i++;
            std::swap(modules[i], modules[j]);
        }
    }
    std::swap(modules[i + 1], modules[high]);
    return i + 1;
}
void quickSort(std::vector<Module *> &modules, int low, int high)
{
    if (low < high)
    {
        int pi = partition(modules, low, high);

        quickSort(modules, low, pi - 1);
        quickSort(modules, pi + 1, high);
    }
}


bool compareModules_r(const Module *a, const Module *b)
{
    if (a->x() == b->x())
    {
        return a->y() < b->y();
    }
    return a->x() > b->x();
}

int partition_r(std::vector<Module *> &modules, int low, int high)
{
    Module *pivot = modules[high];
    int i = low - 1;

    for (int j = low; j < high; ++j)
    {
        if (compareModules_r(modules[j], pivot))
        {
            i++;
            std::swap(modules[i], modules[j]);
        }
    }
    std::swap(modules[i + 1], modules[high]);
    return i + 1;
}
void quickSort_r(std::vector<Module *> &modules, int low, int high)
{
    if (low < high)
    {
        int pi = partition_r(modules, low, high);

        quickSort_r(modules, low, pi - 1);
        quickSort_r(modules, pi + 1, high);
    }
}

bool compareModules_dag(const DAG_Node *a, const DAG_Node *b)
{
    if (a->getX() == b->getX())
    {
        return a->getY() > b->getY();
    }
    return a->getX() < b->getX();
}

int partition_dag(std::vector<DAG_Node *> &modules, int low, int high)
{
    DAG_Node *pivot = modules[high];
    int i = low - 1;

    for (int j = low; j < high; ++j)
    {
        if (compareModules_dag(modules[j], pivot))
        {
            i++;
            std::swap(modules[i], modules[j]);
        }
    }
    std::swap(modules[i + 1], modules[high]);
    return i + 1;
}
void quickSort_dag(std::vector<DAG_Node *> &modules, int low, int high)
{
    if (low < high)
    {
        int pi = partition_dag(modules, low, high);

        quickSort_dag(modules, low, pi - 1);
        quickSort_dag(modules, pi + 1, high);
    }
}


double moduleOverlap_X(Module *i, Module *j)
{
    return (i->width()-(j->x() - i->x()));
    return (i->width()-(j->x() - i->x()));
}
double moduleOverlap_X_r(Module *i, Module *j)
{
    return (j->width()-(i->x() - j->x()));
}
double moduleOverlap_X_r(Module *i, Module *j)
{
    return (j->width()-(i->x() - j->x()));
}
bool compareNodes(const pair<DAG_Node *, double> &a, const pair<DAG_Node *, double> &b)
{
    return a.first == b.first;
}

struct CompareDAGNodePtr
{
    bool operator()(const pair<DAG_Node *, double> &a, const pair<DAG_Node *, double> &b) const
    {
        return a.first < b.first;
    }
};
// clang-format off
bool isDuplicate(vector<pair<DAG_Node *, double> > edges, const pair<DAG_Node *, double> &newPair)
{
    for (size_t i = 0; i < edges.size(); ++i)
    {
        if (compareNodes(edges[i], newPair))
        {
            return true;
        }
    }
    return false;
}
void Placement::construct_DAG_L()
{
    int row_left_boundary = _dataBase->row(0)->x();
    int row_right_boundary = _dataBase->row(0)->x() + (_dataBase->row(0)->numSites()) * _dataBase->row(0)->width();
    int row_top_boundary = _dataBase->row(_dataBase->getNumRows() - 1)->y()+_dataBase->row(_dataBase->getNumRows() - 1)->height();
    int row_bottom_boundary = _dataBase->row(0)->y();
    DAG_Node *L = new DAG_Node();
    L->setName("Left_boundary");
    L->setX(row_left_boundary);
    L->setY(row_top_boundary);
    DAG_Node *R = new DAG_Node();
    R->setName("Right_boundary");
    R->setX(row_right_boundary);
    R->setY(row_top_boundary);

    vector<Module *> modules = _dataBase->getmodule();
    quickSort(modules, 0, modules.size() - 1);
    for (size_t i = 0; i < modules.size(); ++i)
    {
        if (modules[i]->isFF())
        {
            DAG_Node *dag_node = new DAG_Node;
            dag_node->setModule(modules[i]);
            dag_node->setX(modules[i]->x());
            dag_node->setY(modules[i]->y());
            dag_node->set_isFF(true);
            _DAG_nodes.push_back(dag_node);
        }
        else
        { // gate devide left and right
            DAG_Node *dag_node_L = new DAG_Node;
            DAG_Node *dag_node_R = new DAG_Node;

            dag_node_L->setModule(modules[i]);
            dag_node_L->setX(modules[i]->x());
            dag_node_L->setY(modules[i]->y());
            dag_node_L->set_isFF(false);

            dag_node_R->setModule(modules[i]);
            dag_node_R->setX(modules[i]->x() + modules[i]->width());
            dag_node_R->setY(modules[i]->y());
            dag_node_R->set_isFF(false);

            _DAG_nodes.push_back(dag_node_L);
            _DAG_nodes.push_back(dag_node_R);
        }
    }
    double RowWidth = _dataBase->row(0)->width();
    double RowHeight = _dataBase->row(0)->height();
    bool is_find = false;
    for (size_t i = 0, sizetmp = _DAG_nodes.size(); i < sizetmp; ++i)
    {
        int x = static_cast<int>(_DAG_nodes[i]->getX() / RowWidth);
        _x2_DAG_Node[x].push_back(_DAG_nodes[i]);
    }

    // L to immediate right cell
    int x = static_cast<int>(L->getX() / RowWidth);
    _x2_DAG_Node[x].push_back(L);

    is_find = false;
    int i_total_level = row_top_boundary / RowHeight;
    int curr_level = row_bottom_boundary / RowHeight;
    map<int, vector<DAG_Node *> >::iterator it = _x2_DAG_Node.find(L->getX() / RowWidth);
    for (curr_level; curr_level <= i_total_level; ++curr_level)
    {
        if (it != _x2_DAG_Node.end())
        {
            for (int k = 0, sizetmp = it->second.size(); k < sizetmp; ++k)
            {
                if ((it->second[k]->getY() / RowHeight == curr_level) && (it->second[k] != L))
                {
                    double weight = L->getX() - it->second[k]->getX();
                    if (isDuplicate(L->getEdge(), {it->second[k], weight}) == 0)
                    {
                        L->addEdge(it->second[k], weight);
                        it->second[k]->addEdge_r(L, weight);
                    }
                    is_find = true;
                    break;
                }
            }
        }
        if (is_find)
        {
            it = _x2_DAG_Node.find(L->getX() / RowWidth);
            is_find = false;
        }
        else
        { // 這層y沒找到
            ++it;
            if (it == _x2_DAG_Node.end() || (it->first) * RowWidth >= row_right_boundary)
            { // L直接到R
                is_find = true;
            }
            --curr_level;
        }
    }
    // END: L to immediate right cell

    for (size_t i = 0, sizetmp = _DAG_nodes.size(); i < sizetmp; ++i)
    {
        is_find = false;
        if (_DAG_nodes[i]->getModule()->isFF())
        {
            int i_total_level = (_DAG_nodes[i]->getY() + _DAG_nodes[i]->getModule()->height()) / RowHeight;
            if ((int)_DAG_nodes[i]->getModule()->height() % (int)RowHeight != 0)
                i_total_level++;
            int curr_level = _DAG_nodes[i]->getY() / RowHeight;
            map<int, vector<DAG_Node *> >::iterator it = _x2_DAG_Node.find(_DAG_nodes[i]->getX() / RowWidth);
            for (curr_level; curr_level <= i_total_level; ++curr_level)
            {
                if (it != _x2_DAG_Node.end())
                {
                    for (int k = 0, sizetmp = it->second.size(); k < sizetmp; ++k)
                    {
                        if ((it->second[k]->getY() / RowHeight == curr_level) && (it->second[k] != _DAG_nodes[i]))
                        {
                            double weight = moduleOverlap_X(_DAG_nodes[i]->getModule(), it->second[k]->getModule());
                            if (isDuplicate(_DAG_nodes[i]->getEdge(), {it->second[k], weight}) == 0)
                            {
                                _DAG_nodes[i]->addEdge(it->second[k], weight);
                                it->second[k]->addEdge_r(_DAG_nodes[i], weight);
                            }
                            is_find = true;
                            break;
                        }
                    }
                }

                if (is_find)
                {
                    it = _x2_DAG_Node.find(_DAG_nodes[i]->getX() / RowWidth);
                    is_find = false;
                }
                else
                { // 這層y沒找到
                    ++it;
                    if (it == _x2_DAG_Node.end() || (it->first) * RowWidth >= row_right_boundary)
                    {
                        double weight = _DAG_nodes[i]->getX() + _DAG_nodes[i]->getModule()->width() - R->getX();
                        _DAG_nodes[i]->addEdge(R, weight);
                        R->addEdge_r(_DAG_nodes[i], weight);
                        is_find = true;
                    }
                    --curr_level;
                }
            }
        }
        else
        { // gate
            double weight = (-1)*_DAG_nodes[i]->getModule()->width();
            _DAG_nodes[i]->addEdge(_DAG_nodes[i + 1], weight); // L -> R
            _DAG_nodes[i+1]->addEdge_r(_DAG_nodes[i], weight); // L -> R
            int curr_level = _DAG_nodes[i]->getY() / RowHeight;
            int i_total_level = (_DAG_nodes[i]->getY() + _DAG_nodes[i]->getModule()->height()) / RowHeight;
            if ((int)_DAG_nodes[i]->getModule()->height() % (int)RowHeight != 0)
                i_total_level++;

            map<int, vector<DAG_Node *> >::iterator it = _x2_DAG_Node.find(_DAG_nodes[i]->getX() / RowWidth);
            for (curr_level; curr_level <= i_total_level; ++curr_level)
            { // clang-format on
                if (it != _x2_DAG_Node.end())
                {
                    for (int k = 0, sizetmp = it->second.size(); k < sizetmp; ++k)
                    {
                        if ((it->second[k]->getY() / RowHeight == curr_level) && (it->second[k] != _DAG_nodes[i]))
                        {
                            double weight = moduleOverlap_X(_DAG_nodes[i]->getModule(), it->second[k]->getModule());
                            if (isDuplicate(_DAG_nodes[i]->getEdge(), {it->second[k], weight}) == 0)
                            {
                                _DAG_nodes[i + 1]->addEdge(it->second[k], weight); // R連接
                                it->second[k]->addEdge_r(_DAG_nodes[i + 1], weight); // R連接
                            }
                            is_find = true;
                            break;
                        }
                    }
                }
                if (is_find)
                {
                    it = _x2_DAG_Node.find(_DAG_nodes[i]->getX() / RowWidth);
                    is_find = false;
                }
                else
                { // 這層y沒找到
                    ++it;
                    if (it == _x2_DAG_Node.end() || (it->first) * RowWidth >= row_right_boundary)
                    {
                        double weight = _DAG_nodes[i]->getX() + _DAG_nodes[i]->getModule()->width() - R->getX();
                        _DAG_nodes[i + 1]->addEdge(R, weight); // R連接
                        R->addEdge_r(_DAG_nodes[i + 1], weight); // R連接
                        is_find = true;
                    }
                    --curr_level;
                }
            }
            ++i; // BL後面一定是BR，跳過BR
        }
    }
}

void Placement::construct_DAG_R()
{
    int row_left_boundary = _dataBase->row(0)->x();
    int row_right_boundary = _dataBase->row(0)->x() + (_dataBase->row(0)->numSites()) * _dataBase->row(0)->width();
    int row_top_boundary = _dataBase->row(_dataBase->getNumRows() - 1)->y()+_dataBase->row(_dataBase->getNumRows() - 1)->height();
    int row_bottom_boundary = _dataBase->row(0)->y();
    DAG_Node *L = new DAG_Node();
    L->setName("Left_boundary");
    L->setX(row_left_boundary);
    L->setY(row_top_boundary);
    DAG_Node *R = new DAG_Node();
    R->setName("Right_boundary");
    R->setX(row_right_boundary);
    R->setY(row_top_boundary);
    map<int, vector<DAG_Node *>, greater<int> > _x2_DAG_Node_R;
    vector<Module *> modules = _dataBase->getmodule();
    quickSort_r(modules, 0, modules.size() - 1);
    for (size_t i = 0; i < modules.size(); ++i)
    {
        if (modules[i]->isFF())
        {
            DAG_Node *dag_node = new DAG_Node;
            dag_node->setModule(modules[i]);
            dag_node->setX(modules[i]->x());
            dag_node->setY(modules[i]->y());
            dag_node->set_isFF(true);
            _DAG_nodes_reverse.push_back(dag_node);
        }
        else
        { // gate devide left and right
            DAG_Node *dag_node_L = new DAG_Node;
            DAG_Node *dag_node_R = new DAG_Node;

            dag_node_L->setModule(modules[i]);
            dag_node_L->setX(modules[i]->x()+ modules[i]->width());
            dag_node_L->setY(modules[i]->y());
            dag_node_L->set_isFF(false);

            dag_node_R->setModule(modules[i]);
            dag_node_R->setX(modules[i]->x());
            dag_node_R->setY(modules[i]->y());
            dag_node_R->set_isFF(false);

            _DAG_nodes_reverse.push_back(dag_node_R);
            _DAG_nodes_reverse.push_back(dag_node_L);
        }
    }
    double RowWidth = _dataBase->row(0)->width();
    double RowHeight = _dataBase->row(0)->height();
    bool is_find = false;
    for (size_t i = 0, sizetmp = _DAG_nodes_reverse.size(); i < sizetmp; ++i)
    {
        int x = static_cast<int>(_DAG_nodes_reverse[i]->getX() / RowWidth);
        _x2_DAG_Node_R[x].push_back(_DAG_nodes_reverse[i]);
    }

    // R to immediate right cell
    int x = static_cast<int>(R->getX() / RowWidth);
    _x2_DAG_Node_R[x].push_back(R);

    is_find = false;
    int i_total_level = row_top_boundary / RowHeight;
    int curr_level = row_bottom_boundary / RowHeight;
    map<int, vector<DAG_Node *> >::iterator it = _x2_DAG_Node_R.find(R->getX() / RowWidth);
    for (curr_level; curr_level <= i_total_level; ++curr_level)
    {
        if (it != _x2_DAG_Node_R.end())
        {
            for (int k = 0, sizetmp = it->second.size(); k < sizetmp; ++k)
            {
                if ((it->second[k]->getY() / RowHeight == curr_level) && (it->second[k] != R))
                {
                    double weight = it->second[k]->getX() + it->second[k]->getModule()->width() - R->getX();
                    if (isDuplicate(R->getEdge(), {it->second[k], weight}) == 0)
                    {
                        R->addEdge(it->second[k], weight);
                    }
                    is_find = true;
                    break;
                }
            }
        }
        if (is_find)
        {
            it = _x2_DAG_Node_R.find(R->getX() / RowWidth);
            is_find = false;
        }
        else
        { // 這層y沒找到
            ++it;
            if (it == _x2_DAG_Node_R.end() || (it->first) * RowWidth <= row_left_boundary)
            { // R直接到L
                is_find = true;
            }
            --curr_level;
        }
    }
    // END: R to immediate right cell

    for (size_t i = 0, sizetmp = _DAG_nodes_reverse.size(); i < sizetmp; ++i)
    {
        is_find = false;
        if (_DAG_nodes_reverse[i]->getModule()->isFF())
        {
            int i_total_level = (_DAG_nodes_reverse[i]->getY() + _DAG_nodes_reverse[i]->getModule()->height()) / RowHeight;
            if ((int)_DAG_nodes_reverse[i]->getModule()->height() % (int)RowHeight != 0)
                i_total_level++;
            int curr_level = _DAG_nodes_reverse[i]->getY() / RowHeight;
            map<int, vector<DAG_Node *> >::iterator it = _x2_DAG_Node_R.find(_DAG_nodes_reverse[i]->getX() / RowWidth);
            for (curr_level; curr_level <= i_total_level; ++curr_level)
            {
                if (it != _x2_DAG_Node_R.end())
                {
                    for (int k = 0, sizetmp = it->second.size(); k < sizetmp; ++k)
                    {
                        if ((it->second[k]->getY() / RowHeight == curr_level) && (it->second[k] != _DAG_nodes_reverse[i]))
                        {
                            double weight = moduleOverlap_X_r(_DAG_nodes_reverse[i]->getModule(), it->second[k]->getModule());
                            if (isDuplicate(_DAG_nodes_reverse[i]->getEdge(), {it->second[k], weight}) == 0)
                            {
                                _DAG_nodes_reverse[i]->addEdge(it->second[k], weight);
                            }
                            is_find = true;
                            break;
                        }
                    }
                }

                if (is_find)
                {
                    it = _x2_DAG_Node_R.find(_DAG_nodes_reverse[i]->getX() / RowWidth);
                    is_find = false;
                }
                else
                { // 這層y沒找到
                    ++it;
                    if (it == _x2_DAG_Node_R.end() || (it->first) * RowWidth <= row_left_boundary)
                    {
                        double weight = _DAG_nodes_reverse[i]->getX() + _DAG_nodes_reverse[i]->getModule()->width() - R->getX();
                        _DAG_nodes_reverse[i]->addEdge(R, weight);
                        is_find = true;
                    }
                    --curr_level;
                }
            }
        }
        else
        { // gate
            double weight = (-1)*_DAG_nodes_reverse[i]->getModule()->width();
            _DAG_nodes_reverse[i]->addEdge(_DAG_nodes_reverse[i + 1], weight); // R -> L

            int curr_level = _DAG_nodes_reverse[i]->getY() / RowHeight;
            int i_total_level = (_DAG_nodes_reverse[i]->getY() + _DAG_nodes_reverse[i]->getModule()->height()) / RowHeight;
            if ((int)_DAG_nodes_reverse[i]->getModule()->height() % (int)RowHeight != 0)
                i_total_level++;

            map<int, vector<DAG_Node *> >::iterator it = _x2_DAG_Node_R.find(_DAG_nodes_reverse[i]->getX() / RowWidth);
            for (curr_level; curr_level <= i_total_level; ++curr_level)
            { // clang-format on
                if (it != _x2_DAG_Node_R.end())
                {
                    for (int k = 0, sizetmp = it->second.size(); k < sizetmp; ++k)
                    {
                        if ((it->second[k]->getY() / RowHeight == curr_level) && (it->second[k] != _DAG_nodes_reverse[i]))
                        {
                            double weight = moduleOverlap_X_r(_DAG_nodes_reverse[i]->getModule(), it->second[k]->getModule());
                            if (isDuplicate(_DAG_nodes_reverse[i]->getEdge(), {it->second[k], weight}) == 0)
                            {
                                _DAG_nodes_reverse[i + 1]->addEdge(it->second[k], weight); // R連接
                            }
                            is_find = true;
                            break;
                        }
                    }
                }
                if (is_find)
                {
                    it = _x2_DAG_Node_R.find(_DAG_nodes_reverse[i]->getX() / RowWidth);
                    is_find = false;
                }
                else
                { // 這層y沒找到
                    ++it;
                    if (it == _x2_DAG_Node_R.end() || (it->first) * RowWidth <= row_left_boundary)
                    {
                        double weight = _DAG_nodes_reverse[i]->getX() + _DAG_nodes_reverse[i]->getModule()->width() - R->getX();
                        _DAG_nodes_reverse[i + 1]->addEdge(R, weight); // R連接
                        is_find = true;
                    }
                    --curr_level;
                }
            }
            ++i; // BR後面一定是BL，跳過BL
        }
    }
    // test
    // for(int i=0;i<_DAG_nodes_reverse.size();++i)
    // {
    //     cout<<_DAG_nodes_reverse[i]->getModule()->name()<<": ";
    //     for(int j=0;j<_DAG_nodes_reverse[i]->getEdge().size();++j)
    //     {
    //         vector<pair<DAG_Node *,double > > buff = _DAG_nodes_reverse[i]->getEdge();
    //         if(buff[j].first==NULL)
    //         {
    //             cout<<"fuck U~~"<<endl;
    //             return;
    //         }
    //      }
    // }

}
// clang-format off
void Placement::topologicalSortUtil_L(DAG_Node *node, stack<DAG_Node *> &Stack, vector<DAG_Node *> &visited)
{
    visited.push_back(node);

    vector<pair<DAG_Node *, double> > edges = node->getEdge();
    for (vector<pair<DAG_Node *, double> >::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        DAG_Node *adjNode = it->first;
        if (find(visited.begin(), visited.end(), adjNode) == visited.end())
        {
            topologicalSortUtil_L(adjNode, Stack, visited);
        }
    }
    if(node==NULL)
    {
        cout<<"YAHOO"<<endl;
    }
    Stack.push(node);
}

// 計算 DAG 中每個節點的最長路徑
void Placement::calculateLongestPaths_L(vector<DAG_Node *> &nodes)
{
    stack<DAG_Node *> Stack;
    vector<DAG_Node *> visited;

    for (int i = 0; i < nodes.size(); i++)
    {
        nodes[i]->setPreviousNode(NULL);
        //---------------------------
        //---------------------------
        // if(nodes[i]->getModule()->isFF())
        // {
        //      nodes[i]->setwidth(5);     
        // }
        // else
        // {
        //      nodes[i]->setwidth(9000000000);
        // }
        nodes[i]->setwidth(nodes[i]->getModule()->width());
        nodes[i]->setwstar(0);
        nodes[i]->setorder(i);
        nodes[i]->setPreviousGate(NULL);
    }
    // 進行拓撲排序
    for (vector<DAG_Node *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        DAG_Node *node = *it;
        if (find(visited.begin(), visited.end(), node) == visited.end())
        {
            topologicalSortUtil_L(node, Stack, visited);
        }
    }

    // 動態規劃計算最長路徑
    while (!Stack.empty())
    {
        DAG_Node *node = Stack.top();
        Stack.pop();

        if (node->getLongestPath() == -DBL_MAX)
        {
            node->setLongestPath(0);
        }

       vector<pair<DAG_Node *, double> > edges = node->getEdge();
        for (vector<pair<DAG_Node *, double> >::iterator it = edges.begin(); it != edges.end(); ++it) {
        DAG_Node *adjNode = it->first;
        double weight = it->second;

            if(node->isFF()==1)
            {
                adjNode->setPreviousGate(node->getPreviousGate());
                adjNode->FFnum= (node->FFnum+1);
                adjNode->setwstar(node->getwidth()+node->getwstar());
            }
            else
            {
                adjNode->setPreviousGate(node);
                adjNode->FFnum= (node->FFnum);
                adjNode->setwstar(node->getwstar());
            }
            adjNode->setLongestPath(node->getLongestPath() + weight);
            adjNode->setPreviousNode(node);  
            adjNode->record = node->record;
            adjNode->record.insert(node->getorder());
    }

    }

    for (vector<DAG_Node *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        DAG_Node *node = *it;
        if(node->isFF()==1)
        {
            // cout<<"node name: "<<node->getName()<<endl;
            if(node->getPreviousGate()!= NULL)
            {
            // cout<<"CASE1"<<endl;
            // cout<<"Previous Gate: "<<node->getPreviousGate()->getName()<<endl;
             node->set_li( node->getLongestPath() - node->getPreviousGate()->getLongestPath());
            }
            else
            {
                // cout<<"CASE2"<<endl;
                node->set_li(node->getLongestPath());
            }
        }
        else
        {
            // cout<<"CASE3"<<endl;
            node->set_li(-DBL_MAX);
        }
    }

}

void Placement::topologicalSortUtil_R(DAG_Node *node, stack<DAG_Node *> &Stack, vector<DAG_Node *> &visited)
{
    visited.push_back(node);

     vector<pair<DAG_Node *, double> > edges = node->getEdge_r();
    for (vector<pair<DAG_Node *, double> >::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        DAG_Node *adjNode = it->first;
        if (find(visited.begin(), visited.end(), adjNode) == visited.end())
        {
            topologicalSortUtil_R(adjNode, Stack, visited);
        }
    }
    Stack.push(node);
}


void Placement::calculateLongestPaths_R(vector<DAG_Node *> &nodes)
{
    stack<DAG_Node *> Stack;
    vector<DAG_Node *> visited;

    for (int i = 0; i < nodes.size(); i++)
    {
        nodes[i]->setPreviousNode(NULL);
        //---------------------------
        //---------------------------
        // if(nodes[i]->getModule()->isFF())
        // {
        //      nodes[i]->setwidth(5);     
        // }
        // else
        // {
        //      nodes[i]->setwidth(9000000000);
        // }
        nodes[i]->setwidth(nodes[i]->getModule()->width());
        nodes[i]->setwstar(0);
        nodes[i]->setorder(i);
        nodes[i]->setPreviousGate(NULL);
    }
    // 進行拓撲排序
    for (vector<DAG_Node *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        DAG_Node *node = *it;
        if (find(visited.begin(), visited.end(), node) == visited.end())
        {
            topologicalSortUtil_R(node, Stack, visited);
        }
    }

    // 動態規劃計算最長路徑
    while (!Stack.empty())
    {
        DAG_Node *node = Stack.top();
        Stack.pop();

        if (node->getLongestPath() == -DBL_MAX)
        {
            node->setLongestPath(0);
        }

        vector<pair<DAG_Node *, double> > edges = node->getEdge_r();
        for (vector<pair<DAG_Node *, double> >::iterator it = edges.begin(); it != edges.end(); ++it) {
        DAG_Node *adjNode = it->first;
        double weight = it->second;
        if(node==NULL)
        {
            cout<<"YAHOO"<<endl;
        }
            if(node->isFF()==1)
            {
                adjNode->setPreviousGate(node->getPreviousGate());
                adjNode->FFnum= (node->FFnum+1);
                adjNode->setwstar(node->getwidth()+node->getwstar());
            }
            else
            {
                adjNode->setPreviousGate(node);
                adjNode->FFnum= (node->FFnum);
                adjNode->setwstar(node->getwstar());
            }
            adjNode->setLongestPath(node->getLongestPath() + weight);
            adjNode->setPreviousNode(node);  
            adjNode->record = node->record;
            adjNode->record.insert(node->getorder());
    }

    }

    for (vector<DAG_Node *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        DAG_Node *node = *it;
        if(node->isFF()==1)
        {
            // cout<<"node name: "<<node->getName()<<endl;
            if(node->getPreviousGate()!= NULL)
            {
            // cout<<"CASE1"<<endl;
            // cout<<"Previous Gate: "<<node->getPreviousGate()->getName()<<endl;
             node->set_ri( node->getLongestPath() - node->getPreviousGate()->getLongestPath());
            }
            else
            {
                // cout<<"CASE2"<<endl;
                node->set_ri(node->getLongestPath());
            }
        }
        else
        {
            // cout<<"CASE3"<<endl;
            node->set_ri(-DBL_MAX);
        }
    }

}



void Placement::printLongestPath(DAG_Node *node)
{
    if (node == NULL)
        return;
    printLongestPath(node->getPreviousNode());
    cout << node->getName() << " -> ";
}

void Placement::cal_rhoi()
{
    for (int i = 0; i < this->_DAG_nodes.size(); i++)
    {
        // if(i==135752)
        // {
        //             cout<<"-----testing test-----"<<endl;
        //             cout<<"Mname: "<<_DAG_nodes[i]->getModule()->name()<<endl;
        // }
        double rhoi = -DBL_MAX;
        if (_DAG_nodes[i]->isFF() == 0)
        {
            
            //cout<<"NONONO"<<endl;
            rhoi = -DBL_MAX;
            _DAG_nodes[i]->setrhoi(rhoi);
            continue;
        }
        for (int j = 0; j <= i; j++)
        {

            if (i == j)
            {
                rhoi = max(rhoi,double(0));
            }
            else if (_DAG_nodes[i]->record.find(j) == _DAG_nodes[i]->record.end() || _DAG_nodes[j]->isFF() == 0)
            {
                
                rhoi = max(rhoi, -DBL_MAX);    
            }
            else
            {
                 rhoi = max(rhoi,( _DAG_nodes[i]->getwstar()-_DAG_nodes[j]->getwstar() - (_DAG_nodes[i]->getX() - _DAG_nodes[j]->getX()) ));
                //  if(i==135752)
                //  {
                //     //cout<<"CASE3"<<endl;
                //     cout<<_DAG_nodes[i]->getModule()->width()<<endl;
                //     cout<<"update: "<<rhoi<<" wstardiff:"<<_DAG_nodes[i]->getwstar()-_DAG_nodes[j]->getwstar()<<" xdiff: "<<(_DAG_nodes[i]->getX() - _DAG_nodes[j]->getX())<<" ON PATH FF: "<<_DAG_nodes[i]->FFnum-_DAG_nodes[j]->FFnum<<" "<<endl;
                //  }
            
            }
        }
        
        _DAG_nodes[i]->setrhoi(rhoi);

    }
}

struct ThreadData {
    Placement* placement;
    int start;
    int end;
};

void* calculate_thetai(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    Placement* placement = data->placement;
    int start = data->start;
    int end = data->end;

    for(int i = start; i < end; i++) {
        double thetai = -DBL_MAX;
        if(placement->_DAG_nodes[i]->isFF() == 0) {
            thetai = -DBL_MAX;
            placement->_DAG_nodes[i]->setthetai(thetai);
            continue;
        }
        for(int j = i; j < placement->_DAG_nodes.size(); j++) {
            if(i == j) {
                thetai = std::max(thetai, double(0));
            }
            else if(placement->_DAG_nodes[j]->record.find(i) == placement->_DAG_nodes[j]->record.end() || placement->_DAG_nodes[j]->isFF() == 0) {
                thetai = std::max(thetai, -DBL_MAX);    
            }
            else {
                thetai = std::max(thetai, (placement->_DAG_nodes[j]->getwstar() - placement->_DAG_nodes[i]->getwstar() - (placement->_DAG_nodes[j]->getX() - placement->_DAG_nodes[i]->getX())));
            }
        }
        placement->_DAG_nodes[i]->setthetai(thetai);
    }

    return NULL;
}

void Placement::cal_thetai() {
    long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    cout<<"CPU num: "<<num_cpus<<endl;
    const int num_threads = num_cpus/2;  // 可以根據 CPU 核心數調整
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int nodes_per_thread = this->_DAG_nodes.size() / num_threads;

    for(int i = 0; i < num_threads; i++) {
        thread_data[i].placement = this;
        thread_data[i].start = i * nodes_per_thread;
        thread_data[i].end = (i == num_threads - 1) ? this->_DAG_nodes.size() : (i + 1) * nodes_per_thread;

        pthread_create(&threads[i], NULL, calculate_thetai, &thread_data[i]);
    }

    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}



int customCeil(double num)
{
    if (num == static_cast<int>(num))
    {
        return static_cast<int>(num);
    }
    else
    {
        return static_cast<int>(ceil(num));
    }
}

void Placement::Displacement()
{
    for (int i = 0, sizetmp = _dataBase->getNumFF(); i < sizetmp; ++i)
    {
        _name2Module[_dataBase->ff(i)->name()] = _dataBase->ff(i);
    }

    for (int i = 0; i < _DAG_nodes.size(); i++)
    {
        double mui = _DAG_nodes[i]->get_mui();
        int li_yi = _DAG_nodes[i]->get_li() - _DAG_nodes[i]->get_deltaY();
        int ri_yi = _DAG_nodes[i]->get_deltaY() - _DAG_nodes[i]->get_ri();
        int displacement = 0;
        int x = 0;
        if (_DAG_nodes[i]->isFF() == 1)
        {

            if (li_yi > customCeil(mui))
            {
                // cout << "CASE 1: " << _DAG_nodes[i]->getX()<<" "<<li_yi << endl;
                displacement = li_yi;
                x = _DAG_nodes[i]->getX() + li_yi;
                int y = _name2Module[_DAG_nodes[i]->getModule()->name()]->y();
                _name2Module[_DAG_nodes[i]->getModule()->name()]->setPosition(x, y);
                // _DAG_nodes[i]->setX(x);
            }
            else if (ri_yi < customCeil(mui))
            {
                // cout << "CASE 2: " << _DAG_nodes[i]->getX()<<" "<< ri_yi << endl;
                displacement = ri_yi;
                x = _DAG_nodes[i]->getX() + ri_yi;
                int y = _name2Module[_DAG_nodes[i]->getModule()->name()]->y();
                _name2Module[_DAG_nodes[i]->getModule()->name()]->setPosition(x, y);
            }
            else
            {
                // cout << "CASE 3: " << _DAG_nodes[i]->getX()<<" "<< customCeil(mui) << endl;
                displacement = customCeil(mui);
                x = _DAG_nodes[i]->getX() + customCeil(mui);
                int y = _name2Module[_DAG_nodes[i]->getModule()->name()]->y();
                _name2Module[_DAG_nodes[i]->getModule()->name()]->setPosition(x, y);
            }
        }
    }
    return;
}
// clang-format on
void Placement::assignNeedM()
{
    double rowYdis = _dataBase->row(1)->y() - _dataBase->row(0)->y();
    if (rowYdis < 0)
    {
        cout << "error: row isn't in increasing order" << endl;
    }
    unsigned maxrowID = _dataBase->getNumRows() - 1;
    for (size_t i = 0; i < _moduleNeedAss.size(); i++)
    {
        while (_dataBase->row(maxrowID)->y() + _moduleNeedAss[i]->height() > _dataBase->getBoundaryTop())
        {
            maxrowID--;
        }
        double upperY, lowerY;
        double temp = (_moduleNeedAss[i]->x() - _dataBase->row(0)->x());
        if (temp < 0)
        {
            temp = 0;
        }
        int idx = round(temp / _dataBase->row(0)->width());
        if (idx < 0)
        {
            cout << "error: idx < 0" << endl;
        }
        while (idx > _dataBase->row(0)->numSites() - 1)
        {
            idx--;
        }
        int idy = round((_moduleNeedAss[i]->y() - _dataBase->row(0)->y()) / rowYdis);
        if (idy < 0)
        {
            cout << "error: idy < 0" << endl;
        }

        // boundary condition
        int idTop = idy;
        int idBot = idy;
        while (idy > maxrowID)
        {
            idy--;
        }
        while (_dataBase->row(idy)->getModWid() + _moduleNeedAss[i]->cellType()->getWidth() >
               (_dataBase->getBoundaryRight() - _dataBase->getBoundaryLeft()) * modWidThres)
        {
            if (idTop < maxrowID)
            {
                idTop++;
            }
            if (idBot > 0)
            {
                idBot--;
            }
            if (_dataBase->row(idTop)->getModWid() + _moduleNeedAss[i]->cellType()->getWidth() <=
                (_dataBase->getBoundaryRight() - _dataBase->getBoundaryLeft()) * modWidThres)
            {
                idy = idTop;
                break;
            }
            if (_dataBase->row(idBot)->getModWid() + _moduleNeedAss[i]->cellType()->getWidth() <=
                (_dataBase->getBoundaryRight() - _dataBase->getBoundaryLeft()) * modWidThres)
            {
                idy = idBot;
                break;
            }
        }
        _moduleNeedAss[i]->setPosition(_dataBase->row(idy)->x() + idx * _dataBase->row(idy)->width(), _dataBase->row(idy)->y());
        _dataBase->row(idy)->incModWid(_moduleNeedAss[i]->cellType()->getWidth());
    }
}