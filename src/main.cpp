// #include <iostream>

// #include "DatabaseDef.h"
// #include "Placement.h"
// using namespace std;

// int main(int argc, char **argv) {
//     Placement p;
//     Database *db = new Database();
//     p.mainLoop(db);
//     return 0;
// }

#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"

int main(/*int argc, char **argv*/) {
    Database testDTB;
    string path = ("/home/users/thlin/final_project/sampleCase");
    testDTB.parser(path);


    //Module * ptrM = testDTB.module(59905);
    //Pin* currentPin = nullptr;
    //for (int i = 0; i < ptrM->numInPins(); i++)
    //{
    //    if (ptrM->InPin(i)->name().find("D")!=string::npos)
    //    {
    //        currentPin = ptrM->InPin(i);
    //    }
    //    else
    //    {
    //        
    //    }
    //}
   
    //Pin* prePin = testDTB.FindPrePin(currentPin);
    //cout << "Last: "<<ptrM->name()<<"  find:   "<< prePin->module()->name()<<"/"<< prePin->name() << endl;
    //cout << "Done parser!!!" << endl;
    //return 0;
}