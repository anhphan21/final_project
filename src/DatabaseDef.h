#ifndef DATABASEDEF_H
#define DATABASEDEF_H

#include <vector>
#include <map>
using namespace std;

// struct Point2;
class Rectangle;
class Bin;
class Row;
class Pin;
class Net;
class Timing;
class BaseCell;
class FFCell;
class Module;
class FFModule;

class Database;

typedef BaseCell CellType;
typedef vector<CellType*> CellLibrary;
typedef map<unsigned, vector<FFCell*>> FFLLibrary;

typedef vector<Module*> ModuleList;
typedef vector<Pin*> PinList;
typedef vector<Net*> NetList;
typedef vector<Row*> RowList;
typedef vector<vector<Bin*>> BinList;

#endif  // DATABASEDEF_H