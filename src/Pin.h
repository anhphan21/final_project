#ifndef PIN_H
#define PIN_H

#include "DatabaseDef.h"
#include "Module.h"
#include "CellLibrary.h"
#include <cmath>
using namespace std;

class Pin {
public:

	Pin() : _x(-1), _y(-1), _xOffset(-1), _yOffset(-1), _module(nullptr), _net(nullptr), _pinId(-1), _slack(0) {

	}

	Pin(Net *net, Module *module, string &pinName, double xOffset, double yOffset) : _net(net), _module(module),
																					 _name(pinName), _xOffset(xOffset),
																					 _yOffset(yOffset), _x(0), _y(0), _slack(0) {
	}

	string name() const { return _name; }
	double x() const { return _x; }
	double y() const { return _y; }
	double xOffset() const { return _xOffset; }
	double yOffset() const { return _yOffset; }
	Module *module() const { return _module; }
	Net *net() const { return _net; }
	unsigned pinId() const { return _pinId; }
	bool isIOdie() const { return (_module == nullptr); }
	double getSlack() const { return _slack; }

	void setPosition(double x, double y) {
		_x = x;
		_y = y;
	}
	void setOffset(double xOffset, double yOffset) {
		_xOffset = xOffset;
		_yOffset = yOffset;
	}

	void setModulePtr(Module *module) { _module = module; }
	void setNetPtr(Net *net) { _net = net; }
	void setPinId(unsigned pinId) { _pinId = pinId; }
	void setSlack(double slack) { _slack = slack; }

	static double calHPWL(const Pin &pin0, const Pin &pin1) {
		return abs(pin0.x() -pin1.x()) + abs(pin0.y() - pin1.y());
	}

private:
	// variables from benchmark input
	string _name;
	double _x, _y;              // absolute x and y
	double _xOffset, _yOffset;  // offsets from the bottom-left of the module
	Module *_module;            // ptr of the associated module
	Net *_net;                    // ptr to the associated net
	unsigned _pinId;            // Pin ID (preserved for nothing)

	double _slack;
};

#endif  // PIN_H`