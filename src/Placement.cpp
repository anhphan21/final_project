#include "Placement.h"
#include "Node.h"
#include "Database.h"
using namespace std;

void Placement::constructGraph()
{
    int num_node = _diamondINF.size();
    for(int i=0;i<num_node;++i)
    {
        Node *n=new Node;
        n->setFFinNode(_diamondINF[i]);
        _name2Node[n->getFFinNode()->name()]=n;
        _nodes.push_back(n);
    }
    Module *test =_dataBase.getClkNets()[0]->pin(0).module();
    for (int i = 0; i < _dataBase.getNumClkNets(); ++i)
    {
        for (int j = 0; j < _dataBase.getClkNets()[i]->numPins(); ++j)
        {
            if(_dataBase.getClkNets()[i]->pin(j).isIOdie()==1)  //IO pin doesn't map module
                continue;
            if (_dataBase.getClkNets()[i]->pin(j).module()->isFF() == 0)   //we only can merge FF 
                continue;
            else
            {
                Module *module_i_j=_dataBase.getClkNets()[i]->pin(j).module();
                for (int k = j + 1; k < _dataBase.getClkNets()[i]->numPins(); ++k)
                {
                    Module *module_i_k=_dataBase.getClkNets()[i]->pin(k).module();
                    if (module_i_k->isFF() == 0)
                        continue;
                    else if (overlap_ornot(module_i_j, module_i_j->radius(), module_i_k, module_i_k->radius()))
                    { // they are overlapping and in the same clknet
                        _name2Node[module_i_j->name()]->addNeighborPair({_name2Node[module_i_k->name()],0}); //weight is 0(temporary)
                        
                    }
                }
            }
        }
    }
}
double cal_distance(pair<double, double> A, pair<double, double> B)
{
    return sqrt(pow(A.first - B.first, 2) + pow(A.second - B.second, 2));
}

bool overlap_ornot(Module *a, double a_r, Module *b, double b_r)
{
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