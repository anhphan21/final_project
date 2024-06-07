#include <iostream>
#include "DatabaseDef.h"
#include "Placement.h"
int main(int argc, char **argv)
{
    Placement p;
    Database *db = new Database();
    p.mainLoop(db);
    return 0;
}
