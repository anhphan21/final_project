#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <set>
#include "Database.h"
// We declare a Placement every time we are doing merge on a clk net
class Nodeinf
{
   public:
       Nodeinf() {  };
       vector<string> record_str;

};

class Placement
{
public:
   Placement()
   {
       _nodes.resize(1);
       _nodes[0] = NULL;
   };
   void mainLoop();
   void constructFeasible(Module *ff,Rhombus in,vector<Rhombus > out);
   Rhombus findInputRegion(Module *ff);
   vector<Rhombus > findOutputRegion(Module *ff);

   void constructGraph();
   set<set<Module *> > calMaxClique(unsigned clkidx);
   double cal_cost(Module *ff1, Module *ff2);


   NodeList findMST();
   void netListGraph();
   // void netListGraph(Module* moduleptr);
   // methods for design (hyper-graph) construction
   void addNode(Node *node) { _nodes.push_back(node); }
   unsigned getNumNode() { return _nodes.size(); }
   void setNodesize(unsigned size);
   void clearNode();
   // func for MST //////////////
   void DecreaseKeyMST(vector<pair<Node *, pair<double, Node *> > > &heap, unsigned idx, double key);
   pair<Node *, pair<double, Node *> > extractMinMST(vector<pair<Node *, pair<double, Node *> > > &heap);
   void swapNodeMST(vector<pair<Node *, pair<double, Node *> > > &heap, unsigned idx1, unsigned idx2);
   // func for MST //////////////
   // func for mergeFF////////////
   void mergeFFinG();
   void eraseEdge(unsigned idx1, unsigned idx2);
   void merge2FF(unsigned idx1, unsigned idx2, unsigned newffidx);
   void debankFFto1bit(string ffname);
   void whichFFtoChose();
   // get design property
   Node *node(unsigned nodeId) { return _nodes[nodeId]; }
   void setDatabase(Database *dataBase) { _dataBase = dataBase; }
   Database *getDatabase() { return _dataBase; }
   map<string, vector<Module*> >  CLKNetModule;
   vector<pair<vector<string>, string> > latch_record;
private:
   Database *_dataBase;
   // construct graph
   vector<Module *> _diamondINF; // Graph_input
   NodeList _nodes;
   map<string, Node *> _name2Node;
   map<string, int> ModuleTraverseN;
};

#endif // PLACEMENT_H
