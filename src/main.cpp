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

// #include <iostream>

// #include "Database.h"
// #include "DatabaseDef.h"

// int main(int argc, char **argv) {
//     Database testDTB;
//     testDTB.parser(argv[1]);
//     cout << "Done parser!!!" << endl;
//     return 0;
// }

#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"

int main(/*int argc, char **argv*/) {
    Database testDTB;
    string path = ("/home/users/thlin/final_project/testcase1.txt");
    testDTB.parser(path);
    Pin* ptr = testDTB.module(1)->InPin(0);
    Module* ptrM = testDTB.module(59905);
    cout << ptr->name() << endl;
    cout << ptr->module()->name() << endl;
    cout << ptr->net()->name() << endl;
    cout << "Last: "<<ptrM->name()<<"  You find: "<<testDTB.FindPrePin(ptrM)->name() << endl;
    cout << "Done parser!!!" << endl;
    return 0;
}