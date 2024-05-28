#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <cassert>
#include <map>

#include "DatabaseDef.h"
#include "Module.h"
#include "Bin.h"
#include "Pin.h"
#include "Net.h"
using namespace std;

class Database
{
public:
	Database();
	~Database() = default;

	void parser();

	// set
	void setName(string &name) { _name = name; }
	double boundaryTop() const { return _boundaryTop; }
	double boundaryLeft() const { return _boundaryLeft; }
	double boundaryBottom() const { return _boundaryBottom; }
	double boundaryRight() const { return _boundaryRight; }
	double alpha() const { return _alpha; }
	double beta() const { return _beta; }
	double gamma() const { return _gamma; }
	double lambda() const { return _lambda; }

	// methods for design (hyper-graph) construction
	void addModule(Module *module) { _modules.push_back(module); }
	void addFF(Module *ff) { _ffModules.push_back(ff); }
	void addNet(Net *net) { _nets.push_back(net); }
	void addClkNet(Net *clk) { _clkNets.push_back(clk); }
	void addPin(Pin *pin) { _pins.push_back(pin), _pins.back()->setPinId(_pins.size()); }
	void addRow(Row *row) { _rows.push_back(row); }
	void addCellLib(CellType *cellLib) { _cellLib.push_back(cellLib); }
	void addFFLib(FFCell *ffLib, unsigned bitNum) { _ffLib[bitNum].push_back(ffLib); }

	// Bin operation
	void initialBinArray();
	void resetBin();
	void updateBinUtil();

	// get design property
	Module *module(unsigned moduleId) { return _modules[moduleId]; }
	Net *net(unsigned netId) { return _nets[netId]; }
	Pin *pin(unsigned pinId) { return _pins[pinId]; }
	Row *row(unsigned rowId) { return _rows[rowId]; }
	Bin *bin(unsigned colIdx, unsigned rowIdx) { return _bins[colIdx][rowIdx]; }

	Pin *input(unsigned inId)
	{
		assert(inId < _numInput);
		return _pins[inId];
	}

	Pin *output(unsigned outId)
	{
		assert(outId < _numOutput);
		return _pins[_numInput + outId];
	}

	unsigned getNumModules() const { return _modules.size(); }
	unsigned getNumFF() const { return _ffModules.size(); }
	unsigned getNumNets() const { return _nets.size(); }
	unsigned getNumClkNets() const { return _clkNets.size(); }
	unsigned getNumDataNets() const { return getNumNets() - getNumClkNets(); }
	unsigned getNumPins() const { return _pins.size(); }
	unsigned getNumInputs() const { return _numInput; }
	unsigned getNumOutputs() const { return _numOutput; }
	unsigned getNumRows() const { return _rows.size(); }
	double getDeltaDelay() const { return _deltaDelay; }
	double getAlpha() const { return _alpha; }
	double getBeta() const { return _beta; }
	double getGamma() const { return _gamma; }
	double getLambda() const { return _lambda; }

private:
	string _name; // Design Name

	// Design Data
	ModuleList _modules;
	NetList _nets;
	PinList _pins;
	RowList _rows;
	BinList _bins;
	// Caching the list for processing
	ModuleList _ffModules;
	NetList _clkNets;

	// Library
	CellLibrary _cellLib;
	FFLLibrary _ffLib;

	// Design statics
	Rectangle _dieRectangle;
	double _boundaryTop;
	double _boundaryLeft;
	double _boundaryBottom;
	double _boundaryRight;

	size_t _numModules;
	size_t _numNets;
	size_t _numInput;
	size_t _numOutput;

	// Bin statics
	double _binWidth;
	double _binHeight;
	double _binMaxUtil;
	int _numBinCol; // Like x index
	int _numBinRow; // Like y index

	// For FF merging
	double _deltaDelay;
	double _alpha;
	double _beta;
	double _gamma;
	double _lambda;

	// Caching
	map<string, BaseCell *> CellType2Ptr;
	map<string, Module *> ModuleName2Ptr;
	map<string, Net *> Netname2Ptr;
};

#endif // DATABASE_H