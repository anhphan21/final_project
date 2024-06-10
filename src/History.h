#ifndef HISTORY_H
#define HISTORY_H

#include <string>

#include "Pin.h"
using namespace std;

class History {
   public:
    History() {}
    History(string& moduleName, string& PinName, Pin* newPin) : _oldModuleName(moduleName), _oldPinName(PinName), _newPin(newPin) {}
    ~History() {}

    string oldModuleName() const { return _oldModuleName; }
    string oldPinName() const { return _oldPinName; }
    Pin* newPin() const { return _newPin; }
   private:
    string _oldModuleName;
    string _oldPinName;
    Pin* _newPin;
};

#endif