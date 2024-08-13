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
  testDTB.builBestCelltype();
  Placement testGraph;
  testGraph.setDatabase(&testDTB);
  testGraph.debankAllFF();
  for (size_t i = 0; i < testDTB.getNumFF(); i++)
  {
    if (testDTB.ff(i)->getTNS() < 0)
      count++;
  }
  // cout << count << endl;
  testDTB.setPositive_slack();
  // count = 0;
  // for (int i = 0; i < testDTB.getNumFF(); ++i)
  // {
  //   if (testDTB.ff(i)->getTNS() < 0)
  //   {
  //     count++;
  //   }
  // }
  // cout<<testDTB.ff(1518)->getTNS()<<endl;
  // cout << count << endl;





  // testDTB.builBestCelltype();
  // Module *testM = testDTB.module(testDTB.getNumModules() - 1);
  // Placement testGraph;
  // testGraph.setDatabase(&testDTB);
  // testGraph.debankFFto1bit(testM->name());
  // for (size_t i = 0; i < testDTB.getNumFF(); i++)
  // {
  // }

  // testDTB.outputTofile(argv[2]);
  return 0;
}
