#ifndef PLACEMENT_H
#define PLACEMENT_H

#include "Database.h"

class Placement {
public:
	void mainLoop();
	void constructDiamond();
	void constructGraph();
	void findMST();
	void mergeFF();

private:
	Database _dataBase;
};

#endif // PLACEMENT_H
