#ifndef HISTORY_H
#define HISTORY_H

#include <string>

#include "Pin.h"
using namespace std;

class History
{
public:
    History() {}
    History(string &moduleName, string &PinName, Pin *newPin) : _oldModuleName(moduleName), _oldPinName(PinName), _newPin(newPin) {}
    ~History() {}
    void setOldModuleName(string &moduleName) { _oldModuleName = moduleName; }
    void setOldPinName(string &PinName) { _oldPinName = PinName; }
    void setNewPin(Pin *newPin) { _newPin = newPin; }
    string oldModuleName() const { return _oldModuleName; }
    string oldPinName() const { return _oldPinName; }
    Pin *newPin() const { return _newPin; }

private:
    string _oldModuleName;
    string _oldPinName;
    // maybe we don't need pins
    Pin *_newPin;
};

#endif