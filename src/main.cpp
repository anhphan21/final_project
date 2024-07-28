#include <iostream>
#include "Database.h"
#include "DatabaseDef.h"
#include "Placement.h"

int main(int argc, char **argv)
{
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    testDTB.setPositive_slack();
    for (int i = 0; i < testDTB.getNegative_slack().size(); ++i)
    {
        cout << testDTB.getNegative_slack()[i]->slack() << endl;
    }
    // for(int i= 0;i<testDTB.getNumFF();++i)
    // {
    //     cout<<testDTB.ff(i)->name()<<" "<<testDTB.ff(i)->x()<<" "<<testDTB.ff(i)->y() << endl;
    // }

    // testDTB.updateRadius();
    // testDTB.updateInitialSlackInfo();
    // Placement testGraph;
    // testGraph.setDatabase(&testDTB);
    // testGraph.constructGraph();
    // cout << testDTB.totalCost(1) << endl;

    // // print MST ///////////////////////////////////////////////
    // for (size_t i = 0; i < testGraph.getNumNode(); i++)
    // {
    //     cout << "Node " << testGraph.node(i)->getFFinNode()->name() << " has neighbor: ";
    //     map<string, pair<Node *, double>> neighbor = testGraph.node(i)->getneighbormap();
    //     for (const auto &pair : neighbor)
    //     {
    //         cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //     }
    //     cout << endl;
    // }
    // testGraph.mainLoop();
    // for (size_t i = 0; i < testGraph.getDatabase()->getNumModules(); i++)
    // {
    //     cout << "Module name " << testGraph.getDatabase()->module(i)->name() << "  Bit: " << testGraph.getDatabase()->module(i)->cellType()->getnumBit();
    //     cout << " x " << testGraph.getDatabase()->module(i)->x() << " " << "y " << testGraph.getDatabase()->module(i)->y() << endl;
    // }
    // cout << testDTB.totalCost(1) << endl;
    return 0;
}