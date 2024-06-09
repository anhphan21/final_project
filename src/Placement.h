#ifndef PLACEMENT_H
#define PLACEMENT_H

#include "Database.h"
#include "Node.h"
// #include "CellLibrary.h"

class Placement {
public:
	void mainLoop();
	void constructDiamond();
	void constructGraph();
	void findMST();
	void mergeFF();
	
private:
	Database _dataBase;
	//construct graph
	vector<Module *> _diamondINF;//Graph_input 
	NodeList _nodes;
	map<string,Node *> _name2Node;
	//end construct graph
};

#endif // PLACEMENT_H
