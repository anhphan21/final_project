#ifndef PIN_H
#define PIN_H

#include "DatabaseDef.h"
using namespace std;

Database *DatabaseMgr;

class Pin {
public:

	Pin() : _x(-1), _y(-1), _xOffset(-1), _yOffset(-1), _module(nullptr), _net(nullptr), _pinId(-1) {

	}

	Pin(Net *net, Module *module, string &pinName, double xOffset, double yOffset) : _net(net), _module(module),
																					 _name(pinName), _xOffset(xOffset),
																					 _yOffset(yOffset) {
		
	}

	string name() const { return _name; }
	double x() const { return _x; }
	double y() const { return _y; }
	double xOffset() const { return _xOffset; }
	double yOffset() const { return _yOffset; }
	Module *module() const { return _module; }
	Net *net() const { return _net; }
	unsigned pinId() const { return _pinId; }

	void setPosition(double x, double y) {
		_x = x;
		_y = y;
	}
	void setOffset(double xOffset, double yOffset) {
		_xOffset = xOffset;
		_yOffset = yOffset;
	}

	void setModuleId(Module *module) { _module = module; }
	void setNetId(Net *net) { _net = net; }
	void setPinId(unsigned pinId) { _pinId = pinId; }

private:
	// variables from benchmark input
	string _name;
	double _x, _y;              // absolute x and y
	double _xOffset, _yOffset;  // offsets from the bottom-left of the module
	Module *_module;         	// ptr of the associated module
	Net *_net;                	// ptr to the associated net
	unsigned _pinId;        	// Pin ID (preserved for nothing)
};

#endif  // PIN_H`