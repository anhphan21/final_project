#ifndef CellLibrary_H
#define CellLibrary_H

#include <iostream>
#include <string>
#include <vector>
#include <iostream>

#include "DatabaseDef.h"

using namespace std;

class BaseCell {
public:
    BaseCell() : _width(0), _height(0), _isFF(false), _pinNum(-1) {}
    BaseCell(string& name, double width, double height, int pinNum) : _name(name), _width(width), _height(height), _isFF(false), _pinNum(pinNum) {}
    BaseCell(string& name, double width, double height, int pinNum, bool isFF) : _name(name), _width(width), _height(height), _pinNum(pinNum), _isFF(isFF) {}

    void setName(string& name) { _name = name; }
    void setWidth(double width) { _width = width; }
    void setHeight(double height) { _height = height; }
    void setFF(bool isFF) { _isFF = isFF; }
    void setNumPin(int numPin) { _pinNum = numPin; }
    void setPin(string& pinName, pair<double, double> offset, bool isOut) {
        _pinNameList.push_back(pinName);
        _pinOffset.push_back(offset);
        if (isOut)
            _outList.push_back(_pinNameList.size() - 1);
        else
            _inList.push_back(_pinNameList.size() - 1);
    }

    string getName() const { return _name; }
    double getWidth() const { return _width; }
    double getHeight() const { return _height; }
    double getArea() const { return _width * _height; }
    bool isFF() const { return _isFF; }
    int pinNum() const { return _pinNum; }
    string pinName(unsigned idx) const { return _pinNameList[idx]; }
    double pinOffsetX(unsigned idx) const { return _pinOffset[idx].first; }
    double pinOffsetY(unsigned idx) const { return _pinOffset[idx].second; }
    // bool isOut(unsigned idx) const { return _outList[idx]; }
    unsigned getOutNum() const { return _outList.size(); }
    unsigned getInNum() const { return _inList.size(); }
    unsigned outIdx(unsigned idx) const {
        assert(idx < getOutNum());
        return _outList[idx];
    }
    unsigned inIdx(unsigned idx) const {
        assert(idx < getInNum());
        return _inList[idx];
    }

    int getPinIdxFromName(string& pinName) const {
        for (size_t i = 0; i < _pinNum; ++i) {
            if (pinName == _pinNameList[i])
                return i;
        }
        cout << "Pin Name is not in the list !!!" << endl;
        return -1;
    }

    virtual void setPower(double) { cout << "Not FF cell !!!" << endl; }
    virtual void setQdelay(double) { cout << "Not FF cell !!!" << endl; }
    virtual void setClkPin(unsigned) { cout << "Not FF cell !!!" << endl; }

    virtual double clkPinIdx() const {
        cout << "Not FF cell !!!" << endl;
        return -1;
    }

    virtual double getPower() const {
        cout << "Not FF cell !!!" << endl;
        return -1;
    }

    virtual double getQdelay() const {
        cout << "Not FF cell !!!" << endl;
        return -1;
    }

   private:
    string _name;
    double _width;
    double _height;
    bool _isFF;
    unsigned _pinNum;

    vector<unsigned> _outList;
    vector<unsigned> _inList;
    vector<string> _pinNameList;
    vector<pair<double, double>> _pinOffset;
};

class FFCell : public BaseCell {
public:
    FFCell() : BaseCell(), _qDelay(0), _power(0), _numBit(0) {}
    FFCell(string& name, double width, double height, unsigned pinNum, unsigned numBit) : BaseCell(name, width, height, pinNum, true),
        _numBit(numBit),
        _qDelay(0),
        _power(0) {};
    FFCell(double qDelay, double power) : _qDelay(qDelay), _power(power) {}

    // set
    void setPower(double p) { _power = p; }
    void setQdelay(double q) { _qDelay = q; }
    void setClkPin(unsigned idx) { _clkPinIdx = idx; }
    // get
    double getPower() const { return _power; }
    double getQdelay() const { return _qDelay; }
    double clkPinIdx() const { return _clkPinIdx; }
    unsigned numBit() const { return _numBit; }

private:
    unsigned _clkPinIdx;
    unsigned _numBit;
    double _qDelay;
    double _power;
};

#endif