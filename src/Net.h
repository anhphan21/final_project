#ifndef NET_H
#define NET_H

#include <vector>
#include "DatabaseDef.h"
using namespace std;

class Net {
public:
    Net() : _clkFlag(false), _outputIdx(-1) {
    }

    unsigned numPins() const { return _pPins.size(); }
    Pin &pin(unsigned index) { return *_pPins[index]; }  // index: 0 ~ (numPins-1), not Pin id
    Pin &getOutputPin() { return *_pPins[_outputIdx]; }
    bool getclkFlag() const { return _clkFlag;}

    void setNumPins(unsigned numPins) { _pPins.resize(numPins); }
    void setclkFlag(bool flag) { _clkFlag = flag;}
    void addPin(Pin *pPin) { _pPins.push_back(pPin); }
    void clearPins() { _pPins.clear(); }
    void setOutputPins(int outIdx) { _outputIdx = outIdx; }

private:
    vector<Pin *> _pPins;
    bool _clkFlag;      // false for normal net, true for clk
    int _outputIdx;  // Store the index of the input of net
};

#endif  // NET_H