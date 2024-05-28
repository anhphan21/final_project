#ifndef MODULE_H
#define MODULE_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "DatabaseDef.h"
#include "Pin.h"
#include "Rectangle.h"
#include "CellLibrary.h"
using namespace std;

class Module {
public:
    Module() : _x(-1), _y(-1), _isFixed(false), _type(nullptr) {
    }

    Module(string &name, CellType* type, double x, double y)
            : _name(name), _type(type), _x(x), _y(y), _isFixed(false) {

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

    double centerX() const { return _x + width() / 2; }
    double centerY() const { return _y + height() / 2; }
    double area() const { return _type->getArea(); }

    Rectangle rectangle() const { return {_x, _y, _x + width(), _y + height()}; }

    double getPower() const { _type->getPower(); }
    double getQdelay() const { _type->getQdelay(); }

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


    /////////////////////////////////////////////
    // get (for pins of this modules)
    /////////////////////////////////////////////
    unsigned totnumPins() { return numInPins() + numOutPins(); }
    unsigned numInPins() { return _inPins.size(); }
    unsigned numOutPins() { return _outPins.size(); }

    Pin &inPin(unsigned index) {
        assert(index < _inPins.size());
        return *_inPins[index];
    }

    Pin &outPin(unsigned index) {
        assert(index < _outPins.size());
        return *_outPins[index];
    }

    /////////////////////////////////////////////
    // set (for pins of this modules)
    /////////////////////////////////////////////
    void setNumPins(unsigned numInPins, unsigned numOutPins) {
        _inPins.resize(numInPins);
        _outPins.resize(numOutPins);
    }

    void addInPin(Pin *pPin) { _inPins.push_back(pPin); }
    void addOutPin(Pin *pPin) { _outPins.push_back(pPin); }
    void clearPins() {
        _inPins.clear();
        _outPins.clear();
    }

private:
    // variables from benchmark input
    string _name;
    double _x, _y;  //bottom-left coordinate
    bool _isFixed;
    CellType *_type;

    // pins of the module
    vector<Pin *> _inPins;
    vector<Pin *> _outPins;

    // update pin positions
    void updatePinPositions() {
        for (unsigned i = 0, endi = numInPins(); i < endi; ++i) {
            Pin &pin = this->inPin(i);
            pin.setPosition(x() + pin.xOffset(), y() + pin.yOffset());
        }

        for (unsigned i = 0, endi = numOutPins(); i < endi; ++i) {
            Pin &pin = this->outPin(i);
            pin.setPosition(y() + pin.xOffset(), y() + pin.yOffset());
        }
    }
};

#endif