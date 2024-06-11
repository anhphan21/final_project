#include "Database.h"

#include <float.h>

#include <fstream>
#include <sstream>

#include "Pin.h"
#include "Row.h"

using namespace std;

Database::Database()
    : _name(),
      _boundaryTop(-1),
      _boundaryBottom(-1),
      _boundaryLeft(-1),
      _boundaryRight(-1),
      _numModules(0),
      _numNet(0),
      _numInput(-1),
      _numOutput(-1),
      _numBinCol(0),
      _numBinRow(0),
      _binWidth(-1),
      _binHeight(-1),
      _binMaxUtil(-1),
      _alpha(-1),
      _beta(-1),
      _gamma(-1),
      _lambda(-1) {}

void Database::parser(const string &filename)
{
    ifstream file(filename);
    string line;

    while (getline(file, line))
    {
        auto _pos = filename.find_last_of("/");
        if (_pos != string::npos)
            _name = filename.substr(_pos + 1);
        else
            _name = filename;

        istringstream iss(line);
        string keyword;
        iss >> keyword;
        if (keyword == "Alpha" || keyword == "Beta" || keyword == "Gamma" ||
            keyword == "Lambda")
        {
            double data;
            iss >> data;
            if (keyword == "Alpha")
            {
                setAlpha(data);
            }
            else if (keyword == "Beta")
            {
                setBeta(data);
            }
            else if (keyword == "Gamma")
            {
                setGamma(data);
            }
            else if (keyword == "Lambda")
            {
                setLambda(data);
            }
        }
        else if (keyword == "DieSize")
        {
            int left, bottom, right, top;
            iss >> left >> bottom >> right >> top;
            setBoundaryLeft(left);
            setBoundaryBottom(bottom);
            setBoundaryRight(right);
            setBoundaryTop(top);
            updateRectangle();
        }
        else if (keyword == "NumInput")
        {
            iss >> _numInput;
            // map<string, Pin*> IODesignPin;
            for (int i = 0; i < _numInput; ++i)
            {
                getline(file, line);
                istringstream iss(line);
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                Pin *pinptr = new Pin();
                // input(num)->name(type);
                // input(num)->setPosition(x, y);

                pinptr->setPinName(type);
                pinptr->setPosition(x, y);
                pinptr->setModulePtr(nullptr);
                _pins.push_back(pinptr);
                if (type == "clk" || type.find("C") != string::npos)
                {
                    type = "CLK";
                }
                IODesign.insert({type, pinptr});
                // IODesignPin.insert({type, pinptr});
            }
            // PinName2Ptr.insert({"IODesignIn", IODesignPin});
        }
        else if (keyword == "NumOutput")
        {
            iss >> _numOutput;
            // map<string, Pin*> IODesignPin;
            for (int i = 0; i < _numOutput; i++)
            {
                getline(file, line);
                istringstream iss(line);
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                Pin *pinptr = new Pin();
                // output(num)->name(type);
                // output(num)->setPosition(x, y);
                IODesign.insert({type, pinptr});
                pinptr->setPinName(type);
                pinptr->setPosition(x, y);
                pinptr->setModulePtr(nullptr);
                _pins.push_back(pinptr);
                // IODesignPin.insert({type, pinptr});
                IODesign.insert({type, pinptr});
            }
            // PinName2Ptr.insert({"IODesignOut", IODesignPin});
        }
        else if (keyword == "FlipFlop")
        {
            double bitCount, width, height, pinCount;
            string id;

            iss >> bitCount >> id >> width >> height >> pinCount;
            FFCell *FFcellptr =
                new FFCell(id, width, height, pinCount, bitCount);

            string _clkPin;
            double _clkX, _clkY;

            // ensure the size of _ffLib
            addFFLib(FFcellptr, bitCount);
            string temp, name;
            double x, y;
            bool _check;
            for (int i = 0; i < pinCount; ++i)
            {
                getline(file, line);
                istringstream iss(line);
                iss >> temp >> name >> x >> y;

                _check = (name[0] == 'Q') ? true : false;

                if (name.substr(0, 3) == "CLK")
                {
                    _clkPin = name;
                    _clkX = x;
                    _clkY = y;
                    continue;
                }
                FFcellptr->setPin(name, make_pair(x, y), _check);
            }
            FFcellptr->setPin(_clkPin, make_pair(_clkX, _clkY), false);
            FFcellptr->setClkPin(FFcellptr->getInNum() - 1);
            CellType2Ptr[id] = FFcellptr;
        }
        else if (keyword == "Gate")
        {
            double width, height;
            int pinCount = 0;
            string id;

            iss >> id >> width >> height >> pinCount;
            BaseCell *bptr = new BaseCell(id, width, height, pinCount);
            addCellLib(bptr);
            CellType2Ptr.insert({id, bptr});
            string temp, name;
            double x, y;
            bool _check;
            for (int i = 0; i < pinCount; i++)
            {
                getline(file, line);
                istringstream iss(line);

                iss >> temp >> name >> x >> y;
                // bptr->AllPinName.push_back(name);
                // if (name.find("O") != string::npos) {
                //     bptr->setOutput(name, 1);
                //     bptr->setPinOffset(name, pair<double, double>(x, y));

                // } else {
                //     bptr->setPinOffset(name, pair<double, double>(x, y));
                // }
                if (name.size() >= 3)
                    _check = (name.substr(0, 3) == "Out") ? true : false;
                bptr->setPin(name, make_pair(width, height), _check);
            }
            // cout << "Gate" << " " << id << " " << width << " " << height <<
            // endl;
        }
        else if (keyword == "NumInstances")
        {
            // Handle Instances
            iss >> _numModules;
            string temp, name, type;
            double x, y;
            CellType *_type;
            int PinOfMnum;
            Module *currentM;
            for (int i = 0; i < _numModules; ++i)
            {
                getline(file, line);
                istringstream instIss(line);
                // Comment: You can put the variables temp, name, type to
                // outside of the while that you can reuse them without
                // re-declarate them -> save some runtime
                instIss >> temp >> name >> type >> x >> y;
                // cout << "testhere" << name << endl;
                // auto it = CellType2Ptr.find(type);
                // if (type.find("G") != string::npos) {
                // cout << "this is Gate type" << endl;
                auto it = CellType2Ptr.find(type);
                map<string, Pin *> PinOfM;
                if (it == CellType2Ptr.end())
                {
                    // can't find Standard Cell
                    // Comment: Nice idea for checking error of input file
                    // cout << "Test error" << type << endl;
                }
                else
                {
                    _type = CellType2Ptr[type];
                    PinOfMnum = _type->pinNum();
                    currentM = new Module(name, _type, x, y);
                    ModuleName2Ptr[name] = currentM;
                    addModule(currentM);
                    if (_type->isFF())
                        addFF(currentM);
                    for (int i = 0; i < PinOfMnum; ++i)
                    {
                        string PinName = _type->pinName(i);
                        Pin *pinptr = new Pin();
                        pinptr->setPosition(x, y);
                        pinptr->setPinName(PinName);
                        pinptr->setOffset(_type->pinOffsetX(i),
                                          _type->pinOffsetY(i));
                        pinptr->setModulePtr(currentM);

                        addPin(pinptr);
                        currentM->addPin(pinptr);
                    }
                }
            }
        }
        else if (keyword == "NumNets")
        {
            int PinNum;
            string temp, Netname;
            iss >> _numNet;

            string type, FFname, TargetPin;
            Module *_tModule;
            CellType *_type;
            Pin *_tPin;
            Net *netptr;
            bool Isclk;

            for (int i = 0; i < _numNet; i++)
            {
                Isclk = false; // 定義這個Net是clkNet!!!每一條定義一個
                getline(file, line);
                istringstream iss(line);

                iss >> temp >> Netname >> PinNum;
                netptr = new Net();
                netptr->setName(Netname);

                for (int j = 0; j < PinNum; j++)
                {
                    getline(file, line);
                    istringstream piniss(line);
                    piniss >> temp >> type;
                    auto pos = type.find("/");
                    if (pos == type.npos)
                    {
                        // 如果沒有'/'的Net pin角 代表會再IODesign  this is
                        // design pin
                        auto it = IODesign.find(type);

                        if (it == IODesign.end())
                        {
                            // IODesignPin腳 處理跟FF不一樣
                            cout << "No match pin!!" << endl;
                        }
                        else
                        {
                            if (type.find("CLK") != string::npos)
                            {
                                // 找到clk
                                Isclk = true;
                                netptr->setclkFlag(true);
                            }
                            it->second->setNetPtr(netptr);
                            netptr->addPin(it->second);
                            netptr->setOutputPins(j);
                        }
                    }
                    else // 有'/'切割的Net Pin
                    {
                        // cout << "有'/'切割的Net Pin" << endl;
                        FFname = type.substr(0, pos);
                        TargetPin = type.substr(pos + 1);
                        auto it = ModuleName2Ptr.find(FFname);
                        if (it == ModuleName2Ptr.end())
                        {
                            cout << "can't find correspoding module" << endl;
                        }
                        else
                        {
                            // Found Pin
                            _type = it->second->cellType();
                            if ((TargetPin.substr(0, 3) == "OUT") ||
                                (TargetPin[0] == 'Q'))
                            {
                                netptr->setOutputPins(j);
                            }
                            _tModule = it->second;
                            _tPin = _tModule->pin(
                                _type->getPinIdxFromName(TargetPin));
                            _tPin->setNetPtr(netptr);
                            netptr->addPin(_tPin);
                        }
                    }
                }
                addNet(netptr);
                if (Isclk)
                    addClkNet(netptr);
            }
        }
        else if (keyword == "BinWidth" || keyword == "BinHeight" ||
                 keyword == "BinMaxUtil")
        {
            int data;
            iss >> data;
            if (keyword == "BinWidth")
            {
                setBinWidth(data);
            }
            else if (keyword == "BinHeight")
            {
                setBinHeight(data);
            }
            else if (keyword == "BinMaxUtil")
            {
                setBinUtil(data);
            }
        }
        else if (keyword == "PlacementRows")
        {
            double startX, startY, siteWidth, siteHeight, totalNumOfSites;
            iss >> startX >> startY >> siteWidth >> siteHeight >>
                totalNumOfSites;
            Row *_tRow = new Row(startX, startY, siteWidth, siteHeight, totalNumOfSites);
            addRow(_tRow);
        }
        else if (keyword == "DisplacementDelay")
        {
            double delay;
            iss >> delay;
            setDisplacementDelay(delay);
        }
        else if (keyword == "QpinDelay")
        {
            string type;
            double delay;
            iss >> type >> delay;
            CellType *_type = CellType2Ptr[type];
            _type->setQdelay(delay);
        }
        else if (keyword == "TimingSlack")
        {
            string name, Dpin;
            double slack;
            iss >> name >> Dpin >> slack;
            Module *_tModule = ModuleName2Ptr[name];
            CellType *_type = _tModule->cellType();
            Pin *_tDPin = _tModule->pin(_type->getPinIdxFromName(Dpin));
            _tDPin->setSlack(slack);
        }
        else if (keyword == "GatePower")
        {
            string type;
            double power;
            iss >> type >> power;
            BaseCell *_type = CellType2Ptr[type];
            _type->setQdelay(power);
        }
    }
    initialBinArray();
    file.close();
    for (size_t i = 0; i < _pins.size(); i++)
    {
        _pins[i]->setHistory(new History());
        string name = _pins[i]->module()->name();
        _pins[i]->history()->setOldModuleName(name);
        name = _pins[i]->name();
        _pins[i]->history()->setOldPinName(name);
    }
}

void Database::initialBinArray()
{
    // Initial Die Boundary
    assert((_boundaryRight != -1) || (_boundaryLeft != -1) ||
           (_boundaryTop != -1) || (_boundaryBottom != -1));
    // Initial Bin information
    assert((_binWidth != -1) || (_binHeight != -1) || (_binMaxUtil != -1));

    _numBinRow = _dieRectangle.height() / _binHeight;
    _numBinCol = _dieRectangle.width() / _binWidth;

    double _binArea = _binHeight * _binWidth;

    double _xPos = _dieRectangle.left();
    double _yPos = _dieRectangle.bottom();

    // Create bin array
    double _tempLeft, _tempBottom;
    _bins.resize(_numBinCol);
    for (int i = 0; i < _numBinCol; ++i)
    {
        _bins[i].resize(_numBinRow);
        _tempLeft = _xPos + _binWidth * i;
        for (int j = 0; j < _numBinRow; ++j)
        {
            _tempBottom = _yPos + _binHeight * j;
            _bins[i][j] = new Bin(_tempLeft, _tempBottom, _tempLeft + _binWidth,
                                  _tempBottom + _binHeight, _binArea);
        }
    }
}

void Database::resetBin()
{
    for (size_t i = 0; i < _numBinCol; ++i)
    {
        for (size_t j = 0; j < _numBinRow; ++j)
        {
            _bins[i][j]->resetBin();
        }
    }
}

void Database::updateBinUtil()
{
    Module *_tmpModule;
    double _tmpCenterX, _tmpCenterY;
    int _lbinIdx, _bbinIdx, _rbinIdx, _tbinIdx;
    // Reset bin util to 0
    resetBin();
    // Update bin util
    for (size_t i = 0; i < _numModules; ++i)
    {
        _tmpModule = module(i);

        _lbinIdx = _tmpModule->x() / _binWidth;
        _bbinIdx = _tmpModule->y() / _binHeight;
        _rbinIdx = (_tmpModule->x() + _tmpModule->width()) / _binWidth;
        _tbinIdx = (_tmpModule->y() + _tmpModule->height()) / _binHeight;

        _rbinIdx = ((_rbinIdx + 1) >= _numBinCol) ? _numBinCol : (_rbinIdx + 1);
        _tbinIdx = ((_tbinIdx + 1) >= _numBinRow) ? _numBinRow : (_tbinIdx + 1);
        _lbinIdx = (_lbinIdx < 0) ? 0 : _lbinIdx;
        _bbinIdx = (_bbinIdx < 0) ? 0 : _bbinIdx;

        for (int j = _lbinIdx; j < _rbinIdx; ++j)
        {
            for (int k = _bbinIdx; k < _tbinIdx; ++k)
            {
                _bins[j][k]->updateOverlapArea(Rectangle::overlapArea(
                    _tmpModule->rectangle(), *_bins[j][k]));
            }
        }
    }
}

// For timing slack

/**
 * Unmark the D pin of FF to update the slack
 */
void Database::unMarkedDPin()
{
    Module *_tModule;

    for (size_t i = 0, endi = _ffModules.size(); i < endi; ++i)
    {
        _tModule = _ffModules[i];
        for (size_t j = 0, endj = _tModule->numInPins(); j < endj; ++j)
        {
            _tModule->InPin(j)->setVisited(false);
        }
    }
}

/**
 * Update slack value of single D pins
 */
void Database::updateSlack(Pin *ffpin)
{
    // double DisplaceDelay = getDisplacementDelay();

    // if (ffpin->preFFPin() == nullptr) {
    //     double old_slack = ffpin->slack();              // SFFN
    //     Pin* front_pin = ffpin->net()->getOutputPin();  // front pin isn't ff (maybe gate or IOpin)

    //     Pin ff_old_pin;  // before merge
    //     ff_old_pin.setPosition(ffpin->getSlackInfor()->oldX(), ffpin->getSlackInfor()->oldY());

    //     Pin ff_curr_pin;  // after merge
    //     ff_curr_pin.setPosition(ffpin->x(), ffpin->y());

    //     double WL_old = Pin::calHPWL(ff_old_pin, front_pin);
    //     double WL_curr = Pin::calHPWL(ff_curr_pin, front_pin);

    //     ffpin->getSlackInfor()->setSlack(old_slack + DisplaceDelay * (WL_old - WL_curr));
    // }
    // else {
    //     if (ffpin->getSlackInfor()->preFFPin()->isVisited() == 1) {
    //         double old_slack = ffpin->getSlackInfor()->slack();  // SFFN
    //         double old_QpinDelay = ffpin->getSlackInfor()->oldQ();
    //         double new_QpinDelay = ffpin->module()->getQdelay();

    //         Pin front_pin = ffpin->net()->getOutputPin();

    //         if (front_pin.module()->isFF() == 1)  // front pin is ff
    //         {
    //             Pin ff_old_pin;  // before merge
    //             ff_old_pin.setPosition(ffpin->getSlackInfor()->oldX(), ffpin->getSlackInfor()->oldY());
    //             Pin ff_curr_pin;  // after merge
    //             ff_curr_pin.setPosition(ffpin->x(), ffpin->y());
    //             double WL_old = Pin::calHPWL(ff_old_pin, front_pin);
    //             double WL_curr = Pin::calHPWL(ff_curr_pin, front_pin);

    //             ffpin->getSlackInfor()->setSlack(old_slack + (old_QpinDelay - new_QpinDelay) + DisplaceDelay * (WL_old - WL_curr));
    //         } else  // front pin isn't ff (gate)
    //         {
    //             Pin ff_old_pin;  // before merge
    //             ff_old_pin.setPosition(ffpin->getSlackInfor()->oldX(), ffpin->getSlackInfor()->oldY());
    //             Pin ff_curr_pin;  // after merge
    //             ff_curr_pin.setPosition(ffpin->x(), ffpin->y());
    //             double D_WL_old = Pin::calHPWL(ff_old_pin, front_pin);
    //             double D_WL_curr = Pin::calHPWL(ff_curr_pin, front_pin);
    //             Pin Q_pre_pin_out;
    //             double farest_pin = 0;
    //             for (int i = 0; i < ffpin->getSlackInfor()->preFFPin()->net()->numPins(); ++i) {
    //                 if (Pin::calHPWL(*ffpin->getSlackInfor()->preFFPin(), ffpin->getSlackInfor()->preFFPin()->net()->pin(i)) > farest_pin) {
    //                     Q_pre_pin_out.setPosition(ffpin->getSlackInfor()->preFFPin()->net()->pin(i).x(), ffpin->getSlackInfor()->preFFPin()->net()->pin(i).y());
    //                     farest_pin = Pin::calHPWL(*ffpin->getSlackInfor()->preFFPin(), ffpin->getSlackInfor()->preFFPin()->net()->pin(i));
    //                 }
    //             }
    //             Pin Q_ff_old_pin;
    //             Q_ff_old_pin.setPosition(ffpin->getSlackInfor()->preFFPin()->getSlackInfor()->oldX(), ffpin->getSlackInfor()->preFFPin()->getSlackInfor()->oldY());
    //             Pin Q_ff_curr_pin;
    //             Q_ff_curr_pin.setPosition(ffpin->getSlackInfor()->preFFPin()->x(), ffpin->getSlackInfor()->preFFPin()->y());
    //             double Q_WL_old = Pin::calHPWL(Q_ff_old_pin, Q_pre_pin_out);
    //             double Q_WL_curr = Pin::calHPWL(Q_ff_curr_pin, Q_pre_pin_out);
    //             ffpin->getSlackInfor()->setSlack(old_slack + (old_QpinDelay - new_QpinDelay) + DisplaceDelay * (Q_WL_old - Q_WL_curr) + DisplaceDelay * (D_WL_old - D_WL_old));
    //         }
    //     } else {
    //         updateSlack(ffpin->getSlackInfor()->preFFPin());
    //     }
    // }
    Pin *_preFFPin;

    if (!ffpin->isVisited())
    { // Check if we need to update or not ?
        double _displacement = 0;
        Pin *_tPin;
        Net *_preNet;
        _preFFPin = ffpin->preFFPin();

        if (_preFFPin != nullptr)
        {                                // If the previous pin != null -> there is a FF before the current FF
            if (!_preFFPin->isVisited()) // If the pre ff pin didn't update slack, then update it before update this FF
                updateSlack(_preFFPin);
            // Update slack of this D pin
            // #TODO:
            // Calculate the preFF->comb displacement
            _preNet = _preFFPin->net();
            double _max = 0;
            // Find the farest pin to cal the displacement
            for (size_t i = 0, endi = _preNet->numPins(); i < endi; i++)
            {
                if (Pin::calHPWL(*_preFFPin, *_preNet->pin(i)) > _max)
                {
                    _tPin = _preNet->pin(i);
                }
            }
            _displacement += abs(_preFFPin->oldX() - _tPin->x()) + abs(_preFFPin->oldY() - _tPin->y()) - abs(_preFFPin->x() - _tPin->x()) + abs(_preFFPin->y() - _tPin->y());
        }

        if (ffpin->isMoved())
        { // If the FF is moved then update the new slack
            _tPin = ffpin->net()->OutputPin();
            _displacement += abs(ffpin->oldX() - _tPin->x()) + abs(ffpin->oldY() - _tPin->y()) - abs(ffpin->x() - _tPin->x()) + abs(ffpin->y() - _tPin->y());
            ;
        }
        ffpin->setSlack(ffpin->slack() + _displacement * _dDelay + (ffpin->oldQ() - ffpin->module()->cellType()->getQdelay()));
    }
    ffpin->setVisited(true);
}

void Database::updateSlackAll()
{
    // Update slack first
    for (size_t i = 0, endi = _ffModules.size(); i < endi; ++i)
    {
        for (size_t j = 0, endj = _ffModules[i]->numInPins() - 1; j < endj; ++j)
        {
            updateSlack(_ffModules[i]->InPin(j));
        }
    }
    // Update timing slack old
    for (size_t i = 0, endi = _ffModules.size(); i < endi; ++i)
    {
        for (size_t j = 0, endj = _ffModules[i]->numInPins() - 1; j < endj; ++j)
        {
            _ffModules[i]->InPin(j)->updateSlackInfo(_ffModules[i]->getQdelay());
        }
    }
    // Unmarked all the FF
    unMarkedDPin();
}

/**
 * Update radius value of Module
 */
void Database::updateRadius(FFCell *_newType)
{
    // Note: Only merge with the single 2bit FF
    // Assume the previous FF's D pin is fixed
    // For multibit FF, the smallest radius will be consider
    double _newQDelay = _newType->getQdelay();
    // Reset the Slack first
    // unMarkedDPin();
    // Require to update the slack first
    Module *_tModule;
    Pin *_tPin;
    Timing *_tSlack;
    double _dist2PreGate;
    double _nRadius;
    double _tRadius;

    for (size_t i = 0, endi = _ffModules.size(); i < endi; ++i)
    {
        // _radius.clear();
        _tRadius = DBL_MAX;
        _tModule = _ffModules[i];
        for (size_t j = 0, endj = _tModule->numInPins(); j < endj; ++j)
        {
            _tPin = _tModule->InPin(j);
            _tSlack = _tPin->getSlackInfor();
            _dist2PreGate = Pin::calHPWL(*_tPin, *_tPin->net()->getOutputPin());
            _nRadius = (_tSlack->slack() + _tSlack->oldQ() - _newQDelay + _dDelay * _dist2PreGate) / _dDelay;
            if (_nRadius < _tRadius)
                _tRadius = _nRadius;
        }
        _tModule->setRadius(_tRadius);
    }
}

void Database::printResult()
{
    // string _tmp = _name + ".out";
    // fstream _outFile;
    // _outFile.open(_tmp);

    // if (!_outFile.is_open())
    //     cout << "Cannot open output file !!!" << endl;

    // _outFile << "CellInst " << _numModules << endl;
    // Module *_tModule;
    // for (size_t i = 0; i < _numModules; ++i)
    // {
    //     _tModule = _modules[i];
    //     _outFile << "Inst " << _tModule->name() << " " << _tModule->cellType()->getName() << " " << _tModule->x() << " " << _tModule->y() << endl;
    // }

    // for (size_t i = 0, endi = _pinHistory.size(); i < endi; ++i)
    // {
    //     _outFile << _pinHistory[i].oldModuleName() << "/" << _pinHistory[i].oldPinName() << " map " << _pinHistory[i].newPin()->module()->name() << "/" << _pinHistory[i].newPin()->name() << endl;
    // }
}