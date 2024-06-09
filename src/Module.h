#ifndef MODULE_H
#define MODULE_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "CellLibrary.h"
#include "DatabaseDef.h"
#include "Pin.h"
#include "Rectangle.h"
using namespace std;

class Module {
   public:
    Module() : _x(-1), _y(-1), _isFixed(false), _type(nullptr), _radius(0) {
    }

    Module(string &name, CellType *type, double x, double y)
        : _name(name), _type(type), _x(x), _y(y), _isFixed(false), _radius(0) {
    }

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    string name() const { return _name; }
    double x() const { return _x; }  // x coordinates
    double y() const { return _y; }  // y coordinates //(x,y): lower-left point of the block
    double width() const { return _type->getWidth(); }
    double height() const { return _type->getHeight(); }
    bool isFixed() const { return _isFixed; }  // if fixed module, return true
    bool isFF() const { return _type->isFF(); }
    CellType *cellType() const { return _type; }

    double centerX() const { return _x + width() / 2; }
    double centerY() const { return _y + height() / 2; }
    double area() const { return _type->getArea(); }

    Rectangle rectangle() const { return {_x, _y, _x + width(), _y + height()}; }

    double getPower() const { return _type->getPower(); }
    double getQdelay() const { return _type->getQdelay(); }

    double radius() const { return _radius; }

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setName(const string &name) { _name = name; }
    void setPosition(double x, double y) {  // would update the pin positions when you set new position
        _x = x;
        _y = y;
        updatePinPositions();  // update pin positions
    }
    void setCenterPosition(double x, double y) {
        _x = x - width() / 2;
        _y = y - height() / 2;
        updatePinPositions();
    }
    void setIsFixed(bool isFixed) { _isFixed = isFixed; }
    
	// For construct the graph of timing
    void setRadius(double rad) { _radius = rad; }

    /////////////////////////////////////////////
    // Pin information
    /////////////////////////////////////////////
	unsigned totnumPins() const  { return _type->pinNum(); }
	unsigned numOutPins() const { return _type->getOutNum(); }
	unsigned numInPins() const {  return _type->getInNum(); }
    void addPin(Pin *pPin) { _pins.push_back(pPin); }
	Pin* pin(unsigned idx) {
		assert(idx < _pins.size());
        return _pins[idx];
	}

	Pin* InPin(unsigned idx) {
		return _pins[_type->inIdx(idx)];
	}

	Pin* OutPin(unsigned idx) {
		return _pins[_type->outIdx(idx)];
	}

    void clearPins() { _pins.clear(); }

   private:
    // variables from benchmark input
    string _name;
    double _x, _y;  // bottom-left coordinate
    bool _isFixed;
    double _radius;
    CellType *_type;

    // pins of the module
    vector<Pin *> _pins;

    // update pin positions
    void updatePinPositions() {
		Pin* _tPin;
        for (unsigned i = 0, endi = totnumPins(); i < endi; ++i) {
			_tPin = _pins[i];
            _tPin->setPosition(x() + _tPin->xOffset(), y() + _tPin->yOffset());
        }
    }
};
#endif
