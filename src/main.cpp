#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"

int main(int argc, char **argv)
{
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    Placement *p = new Placement();
    p->setDatabase(&testDTB);
    p->merge2FF(1, 2, 0);
    return 0;
}