#include <iostream>
#include "Database.h"
#include "DatabaseDef.h"
#include "Placement.h"
int main(int argc, char **argv)
{
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;

    // Module* ptrM = testDTB.module(2);
    // Pin* currentPin = nullptr;
    // for (int i = 0; i < ptrM->numInPins(); i++) {
    //     if (ptrM->InPin(i)->name().find("D") != string::npos) {
    //         currentPin = ptrM->InPin(i);
    //     } else {
    //     }
    // }
    // testDTB.updateInitialSlackInfo();
    // cout << "input Module: " << ptrM->name() << endl;
    // cout<<"before: "<<currentPin->slack()<<endl;
    // double x_off = 10 + currentPin->x();
    // double y_off = 0 + currentPin->y();
    // currentPin->setPosition(x_off, y_off);
    // testDTB.updateSlack(currentPin);
    // cout<<"after: "<<currentPin->slack()<<endl;
    // for(int i=0;i<testDTB.getNumFF();++i)
    testDTB.updateRadius();
    testDTB.updateInitialSlackInfo();
    Placement testGraph;
    testGraph.setDatabase(&testDTB);
    testGraph.constructGraph();
    cout << testDTB.totalCost(1) << endl;

    // // print MST ///////////////////////////////////////////////
    for (size_t i = 0; i < testGraph.getNumNode(); i++)
    {
        cout << "Node " << testGraph.node(i)->getFFinNode()->name() << " has neighbor: ";
        map<string, pair<Node *, double>> neighbor = testGraph.node(i)->getneighbormap();
        for (const auto &pair : neighbor)
        {
            cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
        }
        cout << endl;
    }
    // // print MST ///////////////////////////////////////////////
    // double cost = 0;
    // double a = testDTB.getAlpha();
    // double b = testDTB.getBeta();
    // double c = testDTB.getGamma();
    // for (int i = 0; i < testGraph.getDatabase()->getNumFF(); ++i)
    // {
    //     cost += b * testGraph.getDatabase()->ff(i)->getPower() + c * testGraph.getDatabase()->ff(i)->area();
    //     for (int j = 0; j < testGraph.getDatabase()->ff(i)->numInPins(); ++j)
    //     {
    //         if (testGraph.getDatabase()->ff(i)->InPin(j)->slack() < 0)
    //         {
    //             cost += a * testGraph.getDatabase()->ff(i)->InPin(j)->slack();
    //         }
    //     }
    // }
    // cout << cost << endl;
    testGraph.mainLoop();
    for (size_t i = 0; i < testGraph.getDatabase()->getNumModules(); i++)
    {
        cout << "Module name " << testGraph.getDatabase()->module(i)->name() << "  Bit: " << testGraph.getDatabase()->module(i)->cellType()->getnumBit();
        cout << " x " << testGraph.getDatabase()->module(i)->x() << " " << "y " << testGraph.getDatabase()->module(i)->y() << endl;
    }
    cout << testDTB.totalCost(1) << endl;

    // cout<<testGraph.getNumNode()<<endl;
    // for(int i=0;i<testGraph.getNumNode();++i)
    // {
    //     cout<<testGraph.node(i)<<" ";
    //     for(int j=0;j<testGraph.node(i)->getNeighborsize();++j)
    //     {
    //         cout<<testGraph.node(i)->getneighborNode(testGraph.node(i))
    //     }
    // }
    return 0;
}