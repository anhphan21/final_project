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

int main(int argc, char **argv) {
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    return 0;
}