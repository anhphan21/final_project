#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"

int main(int argc, char** argv) {
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    Module * ptrM = testDTB.module(2);
    Pin* currentPin = nullptr;
    for (int i = 0; i < ptrM->numInPins(); i++)
    {
        if (ptrM->InPin(i)->name().find("D")!=string::npos)
        {
            currentPin = ptrM->InPin(i);
        }
        else
        {
            
        }
    }
   
    // Pin* prePin = testDTB.FindPrePin(currentPin);
    // cout << "最後一次: "<<ptrM->name()<<"  找到了   "<< prePin->module()->name()<<"/"<< prePin->name() << endl;
    // cout << "Done parser!!!" << endl;


    cout<<"input Module: "<<ptrM->name()<<endl;
    // cout<<"before: "<<currentPin->slack()<<endl;
    double x_off = 10+currentPin->x();
    double y_off = 20+currentPin->y();
    currentPin->setPosition(x_off,y_off);
    testDTB.updateSlack(currentPin);
    // cout<<"after: "<<currentPin->slack()<<endl;
    return 0;
}