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

  // cout << testGraph.cal_total_cost() << endl;
  // testDTB.outputTofile(argv[2]);

  layout(argv[2], testDTB.getBoundaryRight(), testDTB.getBoundaryTop(), testDTB.getmodule());
  return 0;
}
