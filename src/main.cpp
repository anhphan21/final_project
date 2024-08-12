#include <iostream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include "Database.h"
#include "DatabaseDef.h"
// #include "Placement.h"

int main(int argc, char **argv)
{
  Database testDTB;
  testDTB.parser(argv[1]);
  cout << "Done parser!!!" << endl;
  // testDTB.builBestCelltype();
  // Module *testM = testDTB.module(testDTB.getNumModules() - 1);
  // Placement testGraph;
  // testGraph.setDatabase(&testDTB);
  // testGraph.debankFFto1bit(testM->name());
  // for (size_t i = 0; i < testDTB.getNumFF(); i++)
  // {
  // }

  testDTB.outputTofile(argv[2]);
  return 0;
}
