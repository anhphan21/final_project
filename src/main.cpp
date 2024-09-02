#include <iostream>
#include <fstream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include "Database.h"
#include "DatabaseDef.h"
#include "Placement.h"

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
  for (size_t j = 0; j < Cnets.size(); j++)
  {
    // clang-format off
    cout <<"start "<<endl;
    set<set<Module *> > cliques = testGraph.calMaxClique(Cnets[j]);
    cout <<"finish"<<endl;
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
  testGraph.construct_DAG();

  cout << "Done DAG Graph!!!" << endl;
  // testGraph._DAG_nodes.clear();
  // Module *m1 = new Module();
  // Module *m2 = new Module();
  // Module *m3 = new Module();
  // Module *m4 = new Module();
  // Module *m5 = new Module();
  // Module *m6 = new Module();
  // DAG_Node* node1 = new DAG_Node();
  // DAG_Node* node2 = new DAG_Node();
  // DAG_Node* node3 = new DAG_Node();
  // DAG_Node* node4 = new DAG_Node();
  // DAG_Node* node5 = new DAG_Node();
  // DAG_Node* node6 = new DAG_Node();
  // testGraph.getDatabase()->getmodule()[2]->setPosition(0,0);
  // testGraph.getDatabase()->getmodule()[2]->cellType()->setFF(1);
  // node1->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node2->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node3->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node4->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node5->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node6->setModule(testGraph.getDatabase()->getmodule()[2]);

  // node1->setName("N1");
  // node2->setName("N2");
  // node3->setName("N3");
  // node4->setName("N4");
  // node5->setName("N5");
  // node6->setName("N6");

  // node1->addEdge(node2,99);
  // node1->addEdge(node4,9);
  // node2->addEdge(node3,2);
  // node3->addEdge(node5,5);
  // node3->addEdge(node6,100);
  // node4->addEdge(node2,1);
  // node4->addEdge(node3,1);
  // node4->addEdge(node5,6);
  // node4->addEdge(node6,800);
  // node5->addEdge(node6,7);
  // testGraph._DAG_nodes.push_back(node1);
  // testGraph._DAG_nodes.push_back(node2);
  // testGraph._DAG_nodes.push_back(node3);
  // testGraph._DAG_nodes.push_back(node4);
  // testGraph._DAG_nodes.push_back(node5);
  // testGraph._DAG_nodes.push_back(node6);

  // testGraph.getDatabase()->getmodule()[2]->setPosition(0,0);
  // testGraph.getDatabase()->getmodule()[3]->setPosition(0,0);
  // testGraph.getDatabase()->getmodule()[2]->cellType()->setFF(1);
  // testGraph.getDatabase()->getmodule()[3]->cellType()->setFF(0);
  // node1->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node3->setModule(testGraph.getDatabase()->getmodule()[3]);
  // node4->setModule(testGraph.getDatabase()->getmodule()[2]);
  // node2->setModule(testGraph.getDatabase()->getmodule()[3]);
  // node1->setName("N1");
  // node2->setName("N2");
  // node3->setName("N3");
  // node4->setName("N4");
  // node1->addEdge(node2,90);
  // node1->addEdge(node4,70);
  // node2->addEdge(node3,7);
  // node1->addEdge(node3,800);
  // testGraph._DAG_nodes.push_back(node1);
  // testGraph._DAG_nodes.push_back(node2);
  // testGraph._DAG_nodes.push_back(node3);
  // testGraph._DAG_nodes.push_back(node4);

  testGraph.calculateLongestPaths(testGraph._DAG_nodes);

  //   for(int i=0; i< 3; i++)
  //   {
  //     if(testGraph._DAG_nodes[i]->getPreviousNode()==NULL)
  //     {
  //       cout<<"HELLO"<<testGraph._DAG_nodes[i]->getName();
  //     }
  // }
  // cout<<"Done LongestPath"<<endl;
  // testGraph.printLongestPath(node6);
  cout << "Done LongestPath" << endl;

  //  for (std::set<int>::iterator it = node6->record.begin(); it != node6->record.end(); ++it) {
  //       std::cout << *it << " ";
  //   }

  cout << "-----------Rhoi---------------" << endl;
  testGraph.cal_rhoi();
  //  for(int i=0; i < testGraph._DAG_nodes.size();i++)
  // {
  //   if(testGraph._DAG_nodes[i]->isFF()&& testGraph._DAG_nodes[i]->getrhoi()>200000 )
  //   cout<<"node"<<i<<" mName :  "<<testGraph._DAG_nodes[i]->getModule()->name()<<"  "<<testGraph._DAG_nodes[i]->getrhoi()<<" "<<testGraph._DAG_nodes[i]->isFF()<<endl;
  // }
  // for(int i=0; i < testGraph._DAG_nodes.size();i++)
  // {

  //   cout<<"node"<<i<<" :  "<<testGraph._DAG_nodes[i]->getrhoi()<<endl;
  // }
  cout << "Rho_i done!" << endl;

  cout << "-----------thetai---------------" << endl;
  testGraph.cal_thetai();

  cout << "Theta_i done!" << endl;

  testGraph.contour_L();
  cout << "Done Contour_L Graph!!!" << endl;

  testGraph.contour_R();
  cout << "Done Contour_R Graph!!!" << endl;

  testGraph.Displacement();
  cout << "Done displacement Graph!!!" << endl;

  // for(int i=0; i < testGraph._DAG_nodes.size();i++)
  // {
  //   if(testGraph._DAG_nodes[i]->isFF())
  //   {
  //     cout<<"node"<<i+1<<" :  "<<testGraph._DAG_nodes[i]->getrhoi()<<"\t"<<testGraph._DAG_nodes[i]->getthetai()<<"\t";
  //     cout<<testGraph._DAG_nodes[i]->get_li()<<"\t"<<testGraph._DAG_nodes[i]->get_ri()<<endl;
  //   }
  // }
  testDTB.outputTofile(argv[2]);

  // layout(argv[2], testDTB.getBoundaryRight(), testDTB.getBoundaryTop(), testDTB.getbuffer());
  return 0;
}