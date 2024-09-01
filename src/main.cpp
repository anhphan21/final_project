#include <iostream>
#include <fstream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include "Database.h"
#include "DatabaseDef.h"
// #include "Placement.h"

void layout(const string &filename, const int x, const int y, vector<Module *> module)
{
  // Open the output file
  ofstream fout((filename + ".las").c_str(), ios::out);
  if (!fout.is_open())
  {
    cout << "Error: the output file is not opened!!" << endl;
    exit(1);
  }

  // Output something to the output file
  fout << "endheader" << endl;
  fout << "line 0 0 " << x << " " << 0 << " gray" << endl;
  fout << "line 0 0 " << 0 << " " << y << " gray" << endl;
  fout << "line 0 " << y << " " << x << " " << y << " gray" << endl;
  fout << "line " << x << " 0 " << x << " " << y << " gray" << endl;

  for (unsigned int i = 0; i < module.size(); ++i)
  {
    if (module[i]->isFF())
    {
      fout << "rect " << module[i]->x() << " " << module[i]->y() << " "
           << module[i]->x() + module[i]->width() << " " << module[i]->y() + module[i]->height() << " "
           << "gray" << " name " << module[i]->name() << endl;
    }
    else
    {
      fout << "rect " << module[i]->x() << " " << module[i]->y() << " "
           << module[i]->x() + module[i]->width() << " " << module[i]->y() + module[i]->height() << " "
           << "red" << " name " << module[i]->name() << endl;
    }
  }

  // Close the output file
  fout.close();
}

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
  for (size_t i = 0; i < 2; i++)
  {
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
  }

  // testDTB.printResult();
  // for (int i = 0; i < testGraph.getNumNode(); ++i)
  //   cout << testGraph.node(i)->getNeighborsize() << endl;
  // for (unsigned i = 0; i < testDTB.getNumClkNets(); i++)
  // {
  //   NetList Cnets = testDTB.getClkNets();
  //   for (size_t j = 0; j < Cnets.size(); j++)
  //   {
  //     // clang-format off
  //     set<set<Module *> > cliques = testGraph.calMaxClique(Cnets[j]);
  //     cout << "net " << j << endl;
  //     while (testGraph.getmaxCliqesize() != 0)
  //     {
  //       set<Module *> a = testGraph.getLargestCliqSet();
  //       set<Module *> b = testGraph.adjustClique(Cnets[j], a);
  //       testGraph.mergeMulti1bitFF(b);
  //       set<set<Module*> >c = testGraph.getwholeCliq();
  //       for (set<set<Module*> >::iterator it = c.begin(); it != c.end(); ++it)
  //       { // clang-format on
  //         const set<Module *> &mySet = *it;
  //         set<Module *> innerSet = mySet;
  //         cout << "-------------------------------" << endl;
  //         for (set<Module *>::iterator it2 = innerSet.begin(); it2 != innerSet.end(); ++it2)
  //         {
  //           cout << (*it2)->name() << "  ";
  //         }
  //         cout << endl;
  //       }
  //     }
  //     testGraph.clearmaxCliq();
  //   }
  // }
  testGraph.construct_DAG();
  cout << "Done DAG Graph!!!" << endl;

  testGraph.contour_L();
  cout << "Done Contour Graph!!!" << endl;

  // testDTB.outputTofile(argv[2]);

  // layout(argv[2], testDTB.getBoundaryRight(), testDTB.getBoundaryTop(), testDTB.getbuffer());
  return 0;
}
