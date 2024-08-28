#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <set>
#include "Database.h"
#include "DAG.h"
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
   void constructFeasible(Module *ff);
   Rhombus *findInputRegion(Module *ff);
   vector<Rhombus *> findOutputRegion(Module *ff);

   void windows();
   double cal_total_cost();
   

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
   void debankAllFF();
   void debankFFto1bit(string ffname);
   void whichFFtoChose();
   // get design property
   Node *node(unsigned nodeId) { return _nodes[nodeId]; }
   void setDatabase(Database *dataBase) { _dataBase = dataBase; }
   Database *getDatabase() { return _dataBase; }
   map<string, vector<Module*> >  CLKNetModule;
   vector<pair<vector<string>, string> > latch_record;

   //DAG
   DAG_Node *DAGnode(unsigned nodeId){ return _DAG_nodes[nodeId];}
   void add_DAG_nodes(DAG_Node *node) { _DAG_nodes.push_back(node); }
   unsigned getNum_DAG_Node() { return _DAG_nodes.size(); }
   void construct_DAG();
   
private:
   Database *_dataBase;
   // construct graph
   vector<Module *> _diamondINF; // Graph_input
   NodeList _nodes;
   map<string, Node *> _name2Node;
   map<string, int> ModuleTraverseN;

   map<double, map<double, double> > _binMap;
   DAG_NodeList _DAG_nodes;
   
   map<pair<int ,int >,vector<DAG_Node *> > _Position2_DAG_Node;
   map<int , vector<DAG_Node *> > _x2_DAG_Node;
};

#endif // PLACEMENT_H