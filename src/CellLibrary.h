#ifndef CellLibrary_H
#define CellLibrary_H

#include <map>
#include <vector>
#include <string>
#include "DatabaseDef.h"

using namespace std;

class BaseCell {
public:
    BaseCell() : _width(0), _height(0), _isFF(false), _pinNum(-1) {}
    BaseCell(string &name, double width, double height, int pinNum) : _name(name), _width(width), _height(height),
                                                                      _isFF(false), _pinNum(pinNum) {}
    BaseCell(string &name, double width, double height, int pinNum, bool isFF) : _name(name), _width(width),
                                                                                 _height(height), _pinNum(pinNum),
                                                                                 _isFF(isFF) {}

    void setName(string &name) { _name = name; }
    void setWidth(double width) { _width = width; }
    void setHeight(double height) { _height = height; }
    void setFF(bool isFF) { _isFF = isFF; }
    void setNumPin(int numPin) { _pinNum = numPin; }
    void setPinOffset(string &pinName, pair<double, double> offset) { _pinOffset[pinName] = offset; }
    void setOutput(string &PinName, bool isOut) { _isOutPin[PinName] = isOut; }

    string getName() const { return _name; }
    double getWidth() const { return _width; }
    double getHeight() const { return _height; }
    double getArea() const { return _width * _height; }
    bool isFF() const { return _isFF; }
    int getPinNum() const { return _pinNum; }
    pair<double, double> getPinOffset(string &pinName) { return _pinOffset[pinName]; }
    bool isOutPin(string &pinName) { return _isOutPin[pinName]; }
    virtual double getPower() const {
        cout << "Not FF cell !!!" << endl;
    }
    virtual double getQdelay() const {
        cout << "Not FF cell !!!" << endl;
    }
private:
    string _name;
    double _width;
    double _height;
    bool _isFF;
    int _pinNum;

    map<string, bool> _isOutPin;
    map<string, pair<double, double>> _pinOffset;
};

class FFCell : public BaseCell {
public:
    FFCell() : BaseCell(), _qDelay(0), _power(0) {}
    FFCell(string &name, double width, double height, int pinNum) : BaseCell(name, width, height, pinNum, true),
                                                                    _qDelay(0), _power(0) {};
    FFCell(double qDelay, double power) : _qDelay(qDelay), _power(power) {}

    // set
    void setPower(double p) { _power = p; }
    void setQdelay(double q) { _qDelay = q; }
    // get
    double getPower() const { return _power; }
    double getQdelay() const { return _qDelay; }

private:
    double _qDelay;
    double _power;
};

#endif