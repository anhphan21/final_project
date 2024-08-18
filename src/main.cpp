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
  // for (int i = 0; i < testGraph.getNumNode(); ++i)
  //   cout << testGraph.node(i)->getNeighborsize() << endl;
  for (unsigned i = 0; i < testDTB.getNumClkNets(); i++)
  {
    NetList Cnets = testDTB.getClkNets();
    for (size_t j = 0; j < Cnets.size(); j++)
    { // clang-format off
      set<set<Module *> > cliques = testGraph.calMaxClique(Cnets[j]);
      int counter = 0;
      for (set<set<Module*> >::iterator it =cliques.begin(); it != cliques.end(); ++it)
      {
        counter++;
        cout << "set " << counter << " : " << endl;
        for (set<Module*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
        {
          cout << (*it2)->name() << "  " ;
        }
        cout << endl;
      }

      // clang-format on
    }
  }

  // testDTB.outputTofile(argv[2]);
  return 0;
}
