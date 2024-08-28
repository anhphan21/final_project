#include <iostream>
#include <fstream>
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

  testDTB.setPositive_slack();
  cout << "Done positive slack!!!" << endl;

  testDTB.builBestCelltype();
  Placement testGraph;
  testGraph.setDatabase(&testDTB);
  testGraph.debankAllFF();
  cout << "Done debank!!!" << endl;

  testGraph.netListGraph();
  cout << "Done Lily Graph!!!" << endl;

  testGraph.windows();
  cout << "Done Weilun Graph!!!" << endl;

  // for(int i=0;i<testGraph.getNumNode();++i)
  //   cout<<testGraph.node(i)->getNeighborsize()<<endl;

  // testDTB.outputTofile(argv[2]);
  return 0;
}
