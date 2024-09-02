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
    int     set_li(int li){_li = li;}
    int     set_ri(int ri){_ri = ri;}

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
    int      getorder() {return order;}
    void     setorder(int a){ order = a;}
    bool       isFF(){return _m->isFF();}
    double getLongestPath() const {return _longestPath;}
    void setLongestPath(double path) {_longestPath = path;}
    DAG_Node* getPreviousNode() const { return _previousNode; }
    void setPreviousNode(DAG_Node* prevNode) { _previousNode = prevNode; }
    
    set<int>    record;
    int         get_li(){return _li;}
    int         get_ri(){return _ri;}
    double      get_mui() {return _mui;}
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
    int         _li;
    int         _ri;
    double      _mui = (rhoi - thetai) / 2;
    vector<pair<DAG_Node *,double > > _edge;

    double _longestPath;
    DAG_Node* _previousNode;
};

class contour_Node
{
public:
    contour_Node(string name) : _name(name), _next(NULL), _prev(NULL), _Max_x(0), _Max_height(0), _head(NULL), _tail(NULL)
    {
    }
    ~contour_Node();

    void setnext(contour_Node *n) { _next = n; }
    void setprev(contour_Node *p) { _prev = p; }
    void setMax_X(size_t x) { _Max_x = x; }
    void setMax_height(size_t y) { _Max_height = y; }

    contour_Node *getnext() { return _next; }
    contour_Node *getprev() { return _prev; }
    int getMax_X() { return _Max_x; }
    int getMax_height() { return _Max_height; }
    string getName() { return _name; }

    contour_Node *gethead() { return _head; }
    contour_Node *gettail() { return _tail; }

    int findmaxHeight(contour_Node *begin, contour_Node *end)
    {
        contour_Node *b = begin;
        contour_Node *e = end;
        int max = b->getMax_height();
        while (b->getName() != e->getName())
        {
            if (b->getMax_height() > max)
                max = b->getMax_height();
            b = b->getnext();
            if (b == NULL)
                break;
        }
        if (b->getName() == e->getName())
        {
            if (b->getMax_height() > max)
                max = b->getMax_height();
        }
        return max;
    }

    int findmax_X(contour_Node *begin, contour_Node *end)
    {
        contour_Node *b = begin;
        contour_Node *e = end;
        int max = b->getMax_X();
        while (b->getName() != e->getName())
        {
            if (b->getMax_X() > max)
                max = b->getMax_X();
            b = b->getnext();
            if (b == NULL)
                break;
        }
        if (b->getName() == e->getName())
        {
            if (b->getMax_X() > max)
                max = b->getMax_X();
        }
        return max;
    }

    contour_Node *search_big(int y)
    {
        contour_Node *curr = _head;
        if (_head != NULL)
        {
            while (curr != NULL)
            {
                if (curr->getMax_height() >= y)
                {
                    return curr;
                }
                curr = curr->getnext();
            }
        }
        cout<<"fuck1: "<<y<<endl;
        return NULL; // 應該不會到這
    }

    contour_Node *search_small(int y)
    {
        contour_Node *curr = _head;
        if (_head != NULL)
        {
            while (curr != NULL)
            {
                if (curr->getMax_height() > y)
                {
                    return curr;
                }
                curr = curr->getnext();
            }
        }
        cout<<"fuck2"<<endl;
        return NULL; // 應該不會到這
    }

    void insertNode_big(contour_Node *newNode, contour_Node *target)
    {
        contour_Node *curr = _head;
        if (_head == NULL)
        {
            _head = newNode;
            _tail = newNode;
            newNode->setnext(NULL);
            newNode->setprev(NULL);
            return;
        }
        else if (target == _head)
        {
            newNode->setnext(_head);
            _head->setprev(newNode);
            _head = newNode;
            return;
        }
        else if (target == NULL)
        {
            newNode->setprev(_tail);
            _tail->setnext(newNode);
            newNode->setnext(NULL);
            _tail = newNode;
            return;
        }
        while (curr != target)
            curr = curr->getnext();
        newNode->setnext(curr->getnext());
        newNode->setprev(curr);
        curr->setnext(newNode);
        if(newNode->getnext()!=NULL)
            newNode->getnext()->setprev(newNode);
    }

    void insertNode_small(contour_Node *newNode, contour_Node *target)
    {
        contour_Node *curr = _head;
        if (_head == NULL)
        {
            _head = newNode;
            _tail = newNode;
            newNode->setnext(NULL);
            newNode->setprev(NULL);
            return;
        }
        else if (target == _head)
        {
            newNode->setnext(_head);
            _head->setprev(newNode);
            _head = newNode;
            return;
        }
        else if (target == NULL)
        {
            newNode->setprev(_tail);
            _tail->setnext(newNode);
            newNode->setnext(NULL);
            _tail = newNode;
            return;
        }
        while (curr->getnext() != target)
            curr = curr->getnext();
        newNode->setnext(curr->getnext());
        newNode->setprev(curr);
        curr->setnext(newNode);
        if(newNode->getnext()!=NULL)
            newNode->getnext()->setprev(newNode);
    }

    void deleteNode(contour_Node *target)
    {
        if (target == NULL)
        {
            return;
        }

        else if (target == _head)
        {
            if (target->getnext() != NULL)
            {
                _head = target->getnext();
                target->getnext()->setprev(NULL);
            }
            else
            {
                _head = NULL;
                _tail = NULL;
            }
        }
        else if (target == _tail)
        {
            _tail = target->getprev();
            target->getprev()->setnext(NULL);
        }
        else if (target->getprev() != NULL && target->getnext() != NULL)
        {
            target->getprev()->setnext(target->getnext());
            target->getnext()->setprev(target->getprev());
        }

        target->setnext(NULL);
        target->setprev(NULL);
        target = NULL;
        // delete target;
    }

    void display()
    {
        cout << "\ndisplay:\n";
        contour_Node *curr = _head;
        while (curr != NULL)
        {
            cout << curr->getName() << " " << curr->getMax_X() << " " << curr->getMax_height() << endl;
            curr = curr->getnext();
        }
    }

    int legalornot(int outline_y)
    {
        contour_Node *curr = _head;
        int count_illegal = 0;
        while (curr != NULL)
        {
            if (curr->getMax_height() > outline_y)
                count_illegal = count_illegal + 1;
            curr = curr->getnext();
        }
        return count_illegal;
    }

private:
    contour_Node *_next;
    contour_Node *_prev;
    contour_Node *_head;
    contour_Node *_tail;
    int _Max_x;
    int _Max_height;
    string _name;
};