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
  Placement testPlacement;
  testPlacement.setDatabase(&testDTB);
  cout<<testPlacement.cal_total_cost()<<endl;
  // testDTB.outputTofile(argv[2]);
  return -1;
}
