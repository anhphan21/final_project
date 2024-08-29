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
        << "red"<< " name " <<module[i]->name() << endl;
    }
  }

  // Close the output file
  fout.close();
}

int main(int argc, char **argv)
{
  Database testDTB;
  testDTB.parser(argv[1]);
  cout<<testDTB.getNumClkNets()<<endl;
  cout << "Done parser!!!" << endl;
  // testDTB.setPositive_slack();
  // cout << "Done positive slack!!!" << endl;

  // testDTB.buildBestCelltype();
  // Placement testGraph;
  // testGraph.setDatabase(&testDTB);
  // unsigned count = 0;
  // for (size_t i = 0; i < testDTB.getNumFF(); i++)
  // {
  //   if (testDTB.ff(i)->cellType()->numBit() > 1)
  //   {
  //     count += testDTB.ff(i)->cellType()->numBit();
  //   }
  // }
  // cout << "count: " << count << endl;
  // testGraph.debankAllFF();
  // cout << "Done debank!!!" << endl;
  // cout << "count: " << count << endl;
  // cout << "ff num " << testDTB.getNumFF() << endl;
  // cout << testDTB.getmaxLibBit() << endl;
  // cout << "power of 4 1bit FF " << testDTB.getBestCelltype(1)->getPower() * 3 << endl;
  // cout << "power of 1 4bit FF " << testDTB.getBestCelltype(4)->getPower() << endl;
  // cout << "power + area of 4 1bit FF "
  //      << testDTB.getBestCelltype(1)->getPower() * 2 + testDTB.getBestCelltype(1)->getArea() * 2 << endl;
  // cout << "power + area of 1 4bit FF "
  //      << testDTB.getBestCelltype(4)->getPower() + testDTB.getBestCelltype(4)->getArea() << endl;
  // cout << "power + area of 1 2 bit FF "
  //      << testDTB.getBestCelltype(2)->getPower() + testDTB.getBestCelltype(2)->getArea() << endl;
  // testGraph.netListGraph();
  // cout << "Done Lily Graph!!!" << endl;

  // testGraph.windows();
  // cout << "Done Weilun Graph!!!" << endl;
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