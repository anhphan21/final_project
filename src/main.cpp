#include <iostream>
#include <fstream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include "Database.h"
#include "DatabaseDef.h"
#include "Placement.h"

void layout(const string &filename, const int x, const int y, vector<Module> &module)
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
  int m = module.size();
  for (size_t i = 0; i < m; ++i)
  {
    if (module[i].isFF())
    {
      fout << "rect " << module[i].x() << " " << module[i].y() << " "
           << module[i].x() + module[i].width() << " " << module[i].y() + module[i].height() << " gray" << " name " << module[i].name() << "\n";
      //  << "gray" << endl;
      //  << "gray" << " name " << module[i].name() << endl;
    }
    else
    {
      fout << "rect " << module[i].x() << " " << module[i].y() << " "
           << module[i].x() + module[i].width() << " " << module[i].y() + module[i].height() << " red" << " name " << module[i].name() << "\n";
      // fout << "rect " << module[i]->x() << " " << module[i]->y() << " "
      //      << module[i]->x() + module[i]->width() << " " << module[i]->y() + module[i]->height() << " "
      //      << "red" << " name " << module[i]->name() << endl;
    }
  }

  // Close the output file
  fout.close();
}

int main(int argc, char **argv)
{
  Database testDTB;
  Placement testGraph;
  testGraph.setDatabase(&testDTB);
  testDTB.parser(argv[1]);
  cout << "Done parser!!!" << endl;
  // testDTB.setPositive_slack();
  // cout << "Done positive slack!!!" << endl;
  testDTB.buildBestCelltype();
  cout << "11 " << endl;
  testDTB.assignNOTonSite();
  testDTB.outputTofile(argv[2]);
  return -1;

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
  cout << "Done merging! " << endl;
  testDTB.buildEachRowWidth();

  cout << "done assign" << endl;
  testGraph.construct_DAG_L();
  cout << "Done DAG_L Graph!!!" << endl;

  // testGraph.construct_DAG_R();
  // cout << "Done DAG_R Graph!!!" << endl;

  // testGraph.calculateLongestPaths_L(testGraph._DAG_nodes);
  // cout << "Done LongestPath" << endl;
  testGraph.cal_rhoi();
  cout << "Rho_i done!" << endl;

  // testGraph.calculateLongestPaths_R(testGraph._DAG_nodes);
  // cout << "Done LongestPath R" << endl;

  // testGraph.cal_thetai();
  // cout << "Theta_i done!" << endl;

  for (int i = 0; i < testGraph._DAG_nodes.size(); ++i)
  {
    if (testGraph._DAG_nodes[i]->isFF())
    {
      cout << testGraph._DAG_nodes[i]->get_li() << " ";
      cout << testGraph._DAG_nodes[i]->get_ri() << " ";
      cout << testGraph._DAG_nodes[i]->getrhoi() << " ";
      cout << testGraph._DAG_nodes[i]->getthetai() << "\n";
    }
  }
  cout << "----------start cal displacement----------" << endl;
  testGraph.Displacement();
  cout << "Done Displacement!!!" << endl;

  // testDTB.outputTofile(argv[2]);

  ModuleList ttt = testDTB.getmodule();

  vector<Module> mods;
  for (int i = 0; i < ttt.size(); ++i)
  {
    mods.push_back(*ttt[i]);
  }

  layout(argv[2], testDTB.getBoundaryRight(), testDTB.getBoundaryTop(), mods);
  return 0;
}