#ifndef PIN_H
#define PIN_H

#include "DatabaseDef.h"
using namespace std;

class Pin {
public:

    Pin() : _x(-1), _y(-1), _xOffset(-1), _yOffset(-1), _moduleId(-1), _netId(-1) {
    }

    double x() const { return _x; }
    double y() const { return _y; }
    double xOffset() const { return _xOffset; }
    double yOffset() const { return _yOffset; }
    unsigned moduleId() const { return _moduleId; }
    unsigned netId() const { return _netId; }
    unsigned pinId() const { return _pinId; }

    void setPosition(double x, double y) {
        _x = x;
        _y = y;
    }
    void setOffset(double xOffset, double yOffset) {
        _xOffset = xOffset;
        _yOffset = yOffset;
    }
    void setModuleId(unsigned moduleId) {
        _moduleId = moduleId;
    }
    void setNetId(unsigned netId) {
        _netId = netId;
    }
    void setPinId(unsigned pinId) {
        _pinId = pinId;
    }

private:
    // variables from benchmark input
    double _x, _y;              // absolute x and y
    double _xOffset, _yOffset;  // offsets from the center of the module
    unsigned _moduleId;         // id of the associated module
    unsigned _netId;            // id of the associated net
    unsigned _pinId;            // Pin Id
};

#endif  // PIN_H`