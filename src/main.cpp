#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"
#include "Placement.h"
int main(int argc, char** argv) {
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