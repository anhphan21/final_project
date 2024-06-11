#ifndef TIMING_H
#define TIMING_H

#include <utility>

#include "DatabaseDef.h"
#include "Pin.h"
using namespace std;

class Timing {
   public:
    Timing() : _slack(0), _preFFPin(nullptr), _oldPos(make_pair(0, 0)), _oldQ(0) {}
    ~Timing() {}

    void setSlack(double slack) { _slack = slack; }
    void setPreFFPin(Pin *preFFPin) { _preFFPin = preFFPin; }
    void setOldPos(double x, double y) { _oldPos = make_pair(x, y); }
    void setOldQ(double oldQ) { _oldQ = oldQ; }

    double slack() const { return _slack; }
    Pin *preFFPin() const { return _preFFPin; }
    double oldX() const { return _oldPos.first; }
    double oldY() const { return _oldPos.second; }
    double oldQ() const { return _oldQ; }
    
   private:
    double _slack;                 // Slack value
    Pin *_preFFPin;                // Location of the previous FF pin Q in the net
    pair<double, double> _oldPos;  // Old location of pin
    double _oldQ;                  // Old Q delay corresponding to the pre FF in the net
    History _pinHistory;
};

#endif