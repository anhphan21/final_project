#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"

int main(int argc, char **argv)
{
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    return 0;
}