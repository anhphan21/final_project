#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <set>
#include "Database.h"
#include "DAG.h"
#include <stack>
// We declare a Placement every time we are doing merge on a clk net
class Nodeinf
{
public:
   Nodeinf() {};
   vector<string> record_str;
};

class Placement
{
public:
   Placement()
   {
      _nodes.resize(1);
      _nodes[0] = NULL;
      _moduleNeedAss.clear();
   };
   void mainLoop();
   void constructFeasible(Module *ff);

   void cal_rhoi();
   void cal_thetai();
   Rhombus *findInputRegion(Module *ff);
   vector<Rhombus *> findOutputRegion(Module *ff);
   bool overlap_ornot(vector<Rhombus *> &input_rhombus, double &leftBound, double &rightBound, double &botBound, double &topBound);
   void windows();
   void constructGraph();
   void printLongestPath(DAG_Node *node);
   void calculateLongestPaths(std::vector<DAG_Node *> &nodes);
   void topologicalSortUtil(DAG_Node *node, std::stack<DAG_Node *> &Stack, std::vector<DAG_Node *> &visited);
   // clang-format off
   set<set<Module *> > calMaxClique(Net *targetNet);
   set<Module*> adjustClique(Net *targetNet , set<Module*> targetClique);
   // step: decide discard or include 
   //       desi
   // clang-format on
   double cal_cost(Module *ff1, Module *ff2);
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
   // clang-format off
   void DecreaseKeyMST(vector<pair<Node *, pair<double, Node *> > > &heap, unsigned idx, double key);
   pair<Node *, pair<double, Node *> > extractMinMST(vector<pair<Node *, pair<double, Node *> > > &heap);
   
   void swapNodeMST(vector<pair<Node *, pair<double, Node *> > > &heap, unsigned idx1, unsigned idx2);
   // func for MST //////////////
   // func for mergeFF////////////
   void mergeFFinG();
   void eraseEdge(unsigned idx1, unsigned idx2);
   void merge2FF(unsigned idx1, unsigned idx2, unsigned newffidx);
   void mergeMulti1bitFF(set<Module*> ffs);
   void debankAllFF();
   void debankFFto1bit(string ffname);
   void whichFFtoChose();
   // get design property
   Node *node(unsigned nodeId) { return _nodes[nodeId]; }
   void setDatabase(Database *dataBase) { _dataBase = dataBase; }
   Database *getDatabase() { return _dataBase; }
   map<string, vector<Module *> > CLKNetModule;
   vector<pair<vector<string>, string> > latch_record;

   //DAG
   DAG_Node *DAGnode(unsigned nodeId){ return _DAG_nodes[nodeId];}
   void add_DAG_nodes(DAG_Node *node) { _DAG_nodes.push_back(node); }
   unsigned getNum_DAG_Node() { return _DAG_nodes.size(); }
   void construct_DAG();
   int contour_L();
   int contour_R();
   void contourL_HY();
   void contourR_HY();
   unsigned getmaxCliqesize(){ return _maxClique.size(); }
   // clang-format off
   set<Module*> getLargestCliqSet()
   {
      set<Module*> largestSet;
      size_t maxSize = 0;
      for (set<set<Module*> >::iterator it = _maxClique.begin(); it != _maxClique.end(); ++it) 
      {
        if (it->size() > maxSize) {
            maxSize = it->size();
            largestSet = *it;
        }
      }
      return largestSet;
   }
   set<Module*> getaCliqSet()
   {
      set<set<Module*> >::iterator yo = _maxClique.begin();
      if (yo != _maxClique.end()) 
      {
        return *yo;
      }
      else
      {
         return set<Module*>();
      }
   }
   set<set<Module*> > getwholeCliq(){ return _maxClique; }
   void clearmaxCliq() { _maxClique.clear(); }
   
   DAG_NodeList _DAG_nodes;
   void Displacement();
   // row assignment ////////
   void DelmoduleAss(Module* mod)
   {
      vector<Module*>::iterator it = find(_moduleNeedAss.begin(), _moduleNeedAss.end(), mod);
      if (it != _moduleNeedAss.end()) 
      {
         _moduleNeedAss.erase(it);
      }
      return;
   }
   void assignNeedM();
   bool checkwidth();
private:
   Database *_dataBase;
   // construct graph
   vector<Module *> _diamondINF; // Graph_input
   NodeList _nodes;
   map<string, Node *> _name2Node;
   map<string, int> ModuleTraverseN;

   map<double, map<double, double> > _binMap;
   
   DAG_NodeList _fixed_cells;
   map<pair<int ,int >,vector<DAG_Node *> > _Position2_DAG_Node;
   map<int , vector<DAG_Node *> > _x2_DAG_Node;
   set<set<Module *> > _maxClique;


   map<string, Module* > _name2Module;
   // modules needed to be assigned 
   vector<Module*> _moduleNeedAss;
   // clang-format on
};

#endif // PLACEMENT_H