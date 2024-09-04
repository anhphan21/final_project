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
  cout << "Done merging! " << endl;
  testDTB.buildEachRowWidth();
  testGraph.assignNeedM();
  cout << "done assign" << endl;
  testGraph.construct_DAG_L();
  cout << "Done DAG_L Graph!!!" << endl;

  testGraph.construct_DAG_R();
  cout << "Done DAG_R Graph!!!" << endl;
  cout << testGraph._DAG_nodes.size() << endl;


  testGraph._DAG_nodes.clear();
  DAG_Node* node1 = new DAG_Node();
  DAG_Node* node2 = new DAG_Node();
  DAG_Node* node3 = new DAG_Node();
  DAG_Node* node4 = new DAG_Node();
  DAG_Node* node5 = new DAG_Node();
  DAG_Node* node6 = new DAG_Node();
  DAG_Node* node7 = new DAG_Node();
  DAG_Node* node8 = new DAG_Node();
  DAG_Node* node9 = new DAG_Node();
  DAG_Node* node10 = new DAG_Node();
  testGraph.getDatabase()->getmodule()[2]->setPosition(0,0);
  testGraph.getDatabase()->getmodule()[2]->cellType()->setFF(1);
  testGraph.getDatabase()->getmodule()[1]->cellType()->setFF(0);
  node1->setModule(testGraph.getDatabase()->getmodule()[2]);
  node2->setModule(testGraph.getDatabase()->getmodule()[2]);
  node3->setModule(testGraph.getDatabase()->getmodule()[2]);
  node4->setModule(testGraph.getDatabase()->getmodule()[2]);
  node5->setModule(testGraph.getDatabase()->getmodule()[1]);
  node6->setModule(testGraph.getDatabase()->getmodule()[1]);
  node7->setModule(testGraph.getDatabase()->getmodule()[2]);
  node8->setModule(testGraph.getDatabase()->getmodule()[2]);
  node9->setModule(testGraph.getDatabase()->getmodule()[2]);
  node10->setModule(testGraph.getDatabase()->getmodule()[2]);

  node1->setName("L");
  node2->setName("C1");
  node3->setName("C2");
  node4->setName("C3");
  node5->setName("BL");
  node6->setName("BR");
  node7->setName("C4");
  node8->setName("C5");
  node9->setName("C6");
  node10->setName("R");

  node1->addEdge(node2,-5);
  node1->addEdge(node3,-6);
  node2->addEdge(node3,1);
  node3->addEdge(node4,2);
  node3->addEdge(node5,0);
  node4->addEdge(node5,2);
  node5->addEdge(node6,-6);
  node2->addEdge(node7,1);
  node7->addEdge(node8,-2);
  node6->addEdge(node8,2);
  node8->addEdge(node9,2);
  node6->addEdge(node10,-8);
  node9->addEdge(node10,-5);
  testGraph._DAG_nodes.push_back(node1);
  testGraph._DAG_nodes.push_back(node2);
  testGraph._DAG_nodes.push_back(node3);
  testGraph._DAG_nodes.push_back(node4);
  testGraph._DAG_nodes.push_back(node5);
  testGraph._DAG_nodes.push_back(node6);
  testGraph._DAG_nodes.push_back(node7);
  testGraph._DAG_nodes.push_back(node8);
  testGraph._DAG_nodes.push_back(node9);
  testGraph._DAG_nodes.push_back(node10);


  testGraph.calculateLongestPaths(testGraph._DAG_nodes);

   for(int i=0; i < testGraph._DAG_nodes.size();i++)
  {

    cout<<"Name: "<<testGraph._DAG_nodes[i]->getName()<<" Li: "<<testGraph._DAG_nodes[i]->get_li()<<endl;
  }

  cout << "Done LongestPath" << endl;

  testGraph.cal_rhoi();
  cout << "Rho_i done!" << endl;

  testGraph.cal_thetai();
  cout << "Theta_i done!" << endl;

  testGraph.contour_L();
  cout << "Done Contour_L Graph!!!" << endl;

  testGraph.contour_R();
  cout << "Done Contour_R Graph!!!" << endl;

  cout << "----------start cal displacement----------" << endl;
  testGraph.Displacement();
  cout << "Done Displacement!!!" << endl;
  // testDTB.outputTofile(argv[2]);

//   ModuleList ttt = testDTB.getmodule();

//   vector<Module> mods;
//   for(int i = 0; i < ttt.size(); ++i)
//   {
//     mods.push_back(*ttt[i]);
//   }

//   layout(argv[2], testDTB.getBoundaryRight(), testDTB.getBoundaryTop(), mods);
  return 0;
}