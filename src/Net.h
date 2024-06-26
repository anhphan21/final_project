#ifndef NET_H
#define NET_H

#include <string>
#include <vector>

#include "DatabaseDef.h"
using namespace std;

class Net {
   public:
    Net() : _clkFlag(false), _outputIdx(-1) {
    }

    Net(string &name) : _name(name), _clkFlag(false), _outputIdx(-1) {}

    string name() const { return _name; }
    unsigned numPins() const { return _pPins.size(); }
    Pin *pin(unsigned index) { return _pPins[index]; }  // index: 0 ~ (numPins-1), not Pin id
    Pin *getOutputPin() { return _pPins[_outputIdx]; }
    bool clkFlag() const { return _clkFlag; }
    unsigned pinNum() const { return _pPins.size(); }
    unsigned getOutIdx() const { return _outputIdx; }
    Pin *OutputPin() { return _pPins[_outputIdx]; }

    void setName(string &name) { _name = name; }
    void setNumPins(unsigned numPins) { _pPins.resize(numPins); }
    void setclkFlag(bool flag) { _clkFlag = flag; }
    void addPin(Pin *pPin) { _pPins.push_back(pPin); }
    void clearPins() { _pPins.clear(); }
    void setOutputPins(int outIdx) { _outputIdx = outIdx; }
    void erasePin(unsigned index) { _pPins.erase(_pPins.begin() + index); }

   private:
    string _name;
    vector<Pin *> _pPins;
    bool _clkFlag;        // false for normal net, true for clk
    unsigned _outputIdx;  // Store the index of the output pin of net
};

#endif  // NET_H