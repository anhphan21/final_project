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
    bool       isFF(){return _isFF;}
    double getLongestPath() const {return _longestPath;}
    void setLongestPath(double path) {_longestPath = path;}
private:
    Module      *_m;
    string      _name;
    double      _weight;
    double      _x;
    double      _y;
    double      _deltaY;
    bool        _isFF;
    vector<pair<DAG_Node *,double > > _edge;
    double _longestPath;
};
