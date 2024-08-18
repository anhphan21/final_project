#include <iostream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include "Database.h"
#include "DatabaseDef.h"
#include "Placement.h"

int main(int argc, char **argv)
{
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    Placement testGraph;
    testGraph.setDatabase(&testDTB);
    testGraph.netListGraph(); 
    testDTB.outputTofile(argv[2]);
    // testDTB.builBestCelltype();
    // vector<double> PAperbit;
    // for (size_t i = 0; i < testDTB.getFFlibBitsize(); i++)
    // {
    //     cout << "CellType :  " << testDTB.getBestCelltype(pow(2, i))->getName()
    //          << "  Area : " << testDTB.getBestCelltype(pow(2, i))->getArea() << "  Power : "
    //          << testDTB.getBestCelltype(pow(2, i))->getPower() << endl;
    //     cout << "bit num : " << testDTB.getBestCelltype(pow(2, i))->getnumBit() << endl;
    // }
    // double min = __DBL_MAX__;
    // unsigned minidx = 0;
    // for (size_t i = 0; i < testDTB.getNumFF(); i++)
    // {
    //     double temp = (testDTB.ff(i)->getPower() + testDTB.ff(i)->area()) / testDTB.ff(i)->cellType()->numBit();
    //     // cout << "power + area per bit : " << (testDTB.ff(i)->getPower() + testDTB.ff(i)->area()) / testDTB.ff(i)->cellType()->numBit() << endl;
    //     if (temp < min)
    //     {
    //         min = temp;
    //         minidx = i;
    //     }
    // }
    // cout << "min power + area per bit : " << min << endl;
    // cout << "name of ff : " << testDTB.ff(minidx)->name() << endl;
    // cout << "bit of ff : " << testDTB.ff(minidx)->cellType()->numBit() << endl;
    return -1;
    // Placement testGraph;
    // testGraph.setDatabase(&testDTB);
    // testGraph.constructGraph();
    // testGraph.merge2FF(0, 1, 0);
    // string name = "reg5";
    // testGraph.debankFFto1bit(name);
    // cout net =================================================================
    // for (size_t i = 0; i < testDTB.getNumNets(); i++)
    // {
    //     cout << testDTB.net(i)->name() << endl;
    //     if (testDTB.net(i)->OutputPin()->module() != nullptr)
    //     {
    //         cout << "output pin: " << testDTB.net(i)->OutputPin()->module()->name()
    //              << " 's  " << testDTB.net(i)->OutputPin()->name() << endl;
    //     }
    //     else
    //     {
    //         cout << "output pin  " << testDTB.net(i)->OutputPin()->name() << endl;
    //     }

    //     for (size_t j = 0; j < testDTB.net(i)->numPins(); j++)
    //     {
    //         if (testDTB.net(i)->pin(j)->module() != nullptr)
    //         {
    //             cout << " pin " << testDTB.net(i)->pin(j)->module()->name() << " 's"
    //                  << testDTB.net(i)->pin(j)->name() << endl;
    //         }
    //         else
    //         {
    //             cout << " pin " << testDTB.net(i)->pin(j)->name() << endl;
    //         }
    //     }
    // }

    // cout net =================================================================

    // for (size_t i = 0; i < testDTB.getNumPins(); i++)
    // {
    //     if (testDTB.pin(i)->net() == nullptr)
    //     {
    //         cout << "netptr is null!" << endl;
    //         cout << "pin : " << testDTB.pin(i)->name() << endl;
    //         if (testDTB.pin(i)->module() != nullptr)
    //         {
    //             cout << "is on module " << testDTB.pin(i)->module()->name() << endl;
    //         }
    //     }
    // }

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
  /*  testDTB.updateRadius();
    testDTB.updateInitialSlackInfo();*/

    // Placement testGraph2;
    // testGraph2.setDatabase(&testDTB);

    
    // Module* test = testDTB.getStringModule("C65638"); //use C65638 FF be a testcase,should be print C65638 Next level FF
    // testGraph2.netListGraph(test); //Only find NextFF of argument FF 



    // testGraph2.netListGraph();     //all find NextFF


    /*testGraph.constructGraph();*/
    //cout << testDTB.totalCost(1) << endl;

    //// // print MST ///////////////////////////////////////////////
    //for (size_t i = 0; i < testGraph.getNumNode(); i++)
    //{
    //    cout << "Node " << testGraph.node(i)->getFFinNode()->name() << " has neighbor: ";
    //    map<string, pair<Node *, double>> neighbor = testGraph.node(i)->getneighbormap();
    //    for (const auto &pair : neighbor)
    //    {
    //        cout << pair.second.first->getFFinNode()->name() << " " << pair.second.second << " ";
    //    }
    //    cout << endl;
    //}
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
    /*testGraph.mainLoop();
    for (size_t i = 0; i < testGraph.getDatabase()->getNumModules(); i++)
    {
        cout << "Module name " << testGraph.getDatabase()->module(i)->name() << "  Bit: " << testGraph.getDatabase()->module(i)->cellType()->getnumBit();
        cout << " x " << testGraph.getDatabase()->module(i)->x() << " " << "y " << testGraph.getDatabase()->module(i)->y() << endl;
    }
    cout << testDTB.totalCost(1) << endl;*/

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
