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
  testDTB.buildBestCelltype();
  Placement testGraph;
  testGraph.setDatabase(&testDTB);
  testGraph.debankAllFF();
  cout << "Done debank!!!" << endl;
  testGraph.netListGraph();
  cout << "Done Lily Graph!!!" << endl;
  testGraph.windows();
  cout << "Done Weilun Graph!!!" << endl;
  // get clk net list
  NetList Cnets = testDTB.getClkNets();
  for (size_t j = 0; j < Cnets.size(); j++)
  {
    // clang-format off
    set<set<Module *> > cliques = testGraph.calMaxClique(Cnets[j]);
    while (testGraph.getmaxCliqesize() != 0)
    {
      set<Module *> a = testGraph.getLargestCliqSet();
      set<Module *> b = testGraph.adjustClique(Cnets[j], a);
      testGraph.mergeMulti1bitFF(b);
      set<set<Module*> >c = testGraph.getwholeCliq();
      for (set<set<Module*> >::iterator it = c.begin(); it != c.end(); ++it)
      { // clang-format on
        const set<Module *> &mySet = *it;
        set<Module *> innerSet = mySet;
        // cout << "-------------------------------" << endl;
        // for (set<Module *>::iterator it2 = innerSet.begin(); it2 != innerSet.end(); ++it2)
        // {
        //   cout << (*it2)->name() << "  ";
        // }
        // cout << endl;
        innerSet.clear();
      }
      a.clear();
      b.clear();
      c.clear();
    }

    cliques.clear();
    testGraph.clearmaxCliq();
  }
  for (size_t i = 0; i < testDTB.getNumModules(); i++)
  {
    cout << i << endl;
    cout << testDTB.module(i)->cellType() << endl;
    // cout << "name : " << testDTB.module(i)->name() << endl;
  }
  for (size_t i = testDTB.getNumModules() - 1; i >= 108000; i--)
  {
    // cout << "name : " << testDTB.module(i)->name() << endl;
  }

  testDTB.printResult();
  // for (int i = 0; i < testGraph.getNumNode(); ++i)
  //   cout << testGraph.node(i)->getNeighborsize() << endl;
  // for (unsigned i = 0; i < testDTB.getNumClkNets(); i++)
  // {
  //   NetList Cnets = testDTB.getClkNets();
  //   for (size_t j = 0; j < Cnets.size(); j++)
  //   {
  //     cout << "net num " << j + 1 << endl;
  //     // clang-format off
  //     set<set<Module *> > cliques = testGraph.calMaxClique(Cnets[j]);
  //     int counter = 0;
  //     if (cliques.size() == 0)
  //     {
  //       continue;
  //     }
  //     for (set<set<Module*> >::iterator it =cliques.begin(); it != cliques.end(); ++it)
  //     {
  //       counter++;
  //       const set<Module *> &mySet = *it;
  //       set<Module *> innerSet = mySet;
  //       innerSet =  testGraph.adjustClique(Cnets[j],innerSet);
  //       cout << "set " << counter << " : " << endl;
  //       for (set<Module*>::iterator it2 = innerSet.begin(); it2 != innerSet.end(); it2++)
  //       {
  //         cout << (*it2)->name() << "  " ;
  //       }
  //       cout << endl;
  //     }
  //     // clang-format on
  //   }
  // }
  // testDTB.outputTofile(argv[2]);
  return 0;
}
