#ifndef PIN_H
#define PIN_H

#include <cmath>
#include <string>
#include <utility>

#include "CellLibrary.h"
#include "DatabaseDef.h"
#include "History.h"
#include "Module.h"
#include "Timing.h"
using namespace std;

class Pin {
   public:
    // Default
    Pin() : _x(-1), _y(-1), _xOffset(-1), _yOffset(-1), _module(nullptr), _net(nullptr) {
    }
    // For input, output Pin
    Pin(string &name, double x, double y) : _name(name), _x(x), _y(y), _xOffset(0), _yOffset(0), _module(nullptr), _net(nullptr), _marked(0) {}
    // For module pin
    Pin(string name, Module *module, double x, double y, double xOffset, double yOffset) : _name(name), _x(x), _y(y), _xOffset(xOffset), _yOffset(yOffset), _module(module), _net(nullptr), _marked(false) {}

    Pin(Net *net, Module *module, string &pinName, double xOffset, double yOffset) : _net(net), _module(module), _name(pinName), _xOffset(xOffset), _yOffset(yOffset), _x(0), _y(0) {
    }

    string name() const { return _name; }
    double x() const { return _x; }
    double y() const { return _y; }
    double xOffset() const { return _xOffset; }
    double yOffset() const { return _yOffset; }
    Module *module() const { return _module; }
    Net *net() const { return _net; }
    History *history() const { return _history; }
    // unsigned pinId() const { return _pinId; }
    bool isIOdie() const { return (_module == nullptr); }
    Timing *getSlackInfor() { return &_slackInfo; }
    bool isVisited() const { return _marked; }

    void setPinName(string &name) { _name = name; }

    void setPosition(double x, double y) {
        _x = x;
        _y = y;
    }
    void setOffset(double xOffset, double yOffset) {
        _xOffset = xOffset;
        _yOffset = yOffset;
    }

    void setHistory(History *history) { _history = history; }
    void setModulePtr(Module *module) { _module = module; }
    void setNetPtr(Net *net) { _net = net; }
    // void setPinId(unsigned pinId) { _pinId = pinId; }
    void setVisited(bool marked) { _marked = marked; }

    bool isMoved() const { return (_slackInfo.oldX() != _x) || (_slackInfo.oldY() != _y); }

    // For slack Information
    void setSlack(double slack) { _slackInfo.setSlack(slack); }
    void setPreFFPin(Pin *preFFPin) { _slackInfo.setPreFFPin(preFFPin); }
    void setOldPos(double x, double y) { _slackInfo.setOldPos(x, y); }
    void setOldQ(double oldQ) { _slackInfo.setOldQ(oldQ); }

    double slack() const { return _slackInfo.slack(); }
    Pin *preFFPin() const { return _slackInfo.preFFPin(); }
    double oldX() const { return _slackInfo.oldX(); }
    double oldY() const { return _slackInfo.oldY(); }
    double oldQ() const { return _slackInfo.oldQ(); }

    /**
     * Check if D pin need to update slack ?
     */
    bool needUpdate() const {
        if (_slackInfo.preFFPin())
            return isMoved();
        else
            return isMoved() || _slackInfo.preFFPin()->isMoved();
    }

    void updateSlackInfo(double qDelay) {
        _slackInfo.setOldPos(_x, _y);
        _slackInfo.setOldQ(qDelay);
    }

    static double calHPWL(const Pin &pin0, const Pin &pin1) {
        return abs(pin0.x() - pin1.x()) + abs(pin0.y() - pin1.y());
    }

   private:
    // variables from benchmark input
    string _name;
    double _x, _y;              // absolute x and y
    double _xOffset, _yOffset;  // offsets from the bottom-left of the module
    Module *_module;            // ptr of the associated module
    Net *_net;                  // ptr to the associated net

    bool _marked;  // use for checking the flipflop timing
    Timing _slackInfo;

    History *_history;
};

#endif  // PIN_H`