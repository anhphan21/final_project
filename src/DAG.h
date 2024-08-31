#include "Database.h"
#include "Placement.h"
using namespace std;

class DAG_Node
{
public:
    DAG_Node():_m(NULL),_deltaY(0)
    {
        _edge.clear();
        for(int i=0;i<_edge.size();++i)
            _edge[i].first=NULL;
    }
    ~DAG_Node();
    void    setModule(Module *m){_m = m;}
    void    setName(string n){_name = n;}
    void    addEdge(DAG_Node *node, double weight){_edge.push_back({node, weight});}
    void    setX(double x){_x = x;}
    void    setY(double y){_y = y;}
    void    set_deltaY(double y){_deltaY = y;}
    bool    set_isFF(bool isFF){_isFF = isFF;}

    vector<pair<DAG_Node *,double > > getEdge(){return _edge;}
    Module *getModule(){return _m;}
    string  getName(){return _name;}
    double     getX(){return _x;}
    double     getY(){return _y;}
    double     get_deltaY(){return _deltaY;}
    void     setwstar (double a) {wstar= a;}
    double   getwstar ()         {return wstar;}
    void     setwidth (double a) {width= a;}
    double   getwidth ()         {return width;}
    double   getrhoi() {return rhoi;}
    double   setrhoi(double a) {rhoi = a;}
    double   getthetai(){return thetai;}
    double   setthetai(double a) {thetai = a;}
    int getorder() {return order;}
    void setorder(int a){ order = a;}
    bool       isFF(){return _m->isFF();}
    double getLongestPath() const {return _longestPath;}
    void setLongestPath(double path) {_longestPath = path;}
    DAG_Node* getPreviousNode() const { return _previousNode; }
    void setPreviousNode(DAG_Node* prevNode) { _previousNode = prevNode; }
    
    set<int> record;

private:
    Module      *_m;
    string      _name;
    int order;
    double      width;
    double      wstar;
    double      _weight;
    double      rhoi;
    double      thetai;
    double      _x;
    double      _y;
    double      _deltaY=0;
    bool        _isFF;
    vector<pair<DAG_Node *,double > > _edge;

    double _longestPath;
    DAG_Node* _previousNode;
};
