#include <iostream>
#include "DatabaseDef.h"
#include "Placement.h"
int main(int argc, char **argv)
{
    Placement p;
    if (argc < 2)
    {
        cout << "error: no input file\n";
        return -1;
    }
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << testDTB.getNumModules() << "\n";
    p.setDatabase(&testDTB);
    p.merge2FF(0, 1, 0);
    return 0;
}
