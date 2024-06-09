#include "Database.h"

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

void Database::parser(const string& filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        auto _pos = filename.find_last_of("/");
        if (_pos != string::npos)
            _name = filename.substr(_pos + 1);
        else
            _name = filename;

        istringstream iss(line);
        string keyword;
        iss >> keyword;
        if (keyword == "Alpha" || keyword == "Beta" || keyword == "Gamma" || keyword == "Lambda") {
            double data;
            iss >> data;
            if (keyword == "Alpha") {
                setAlpha(data);
            } else if (keyword == "Beta") {
                setBeta(data);
            } else if (keyword == "Gamma") {
                setGamma(data);
            } else if (keyword == "Lambda") {
                setLambda(data);
            }
        } else if (keyword == "DieSize") {
            int left, bottom, right, top;
            iss >> left >> bottom >> right >> top;
            setBoundaryLeft(left);
            setBoundaryBottom(bottom);
            setBoundaryRight(right);
            setBoundaryTop(top);
            updateRectangle();
        } else if (keyword == "NumInput") {
            double num;
            iss >> num;
            // map<string, Pin*> IODesignPin;
            for (int i = 0; i < num; i++) {
                getline(file, line);
                istringstream iss(line);
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                Pin* pinptr = new Pin();
                // input(num)->name(type);
                // input(num)->setPosition(x, y);

                pinptr->setPinName(type);
                pinptr->setPosition(x, y);
                pinptr->setModulePtr(nullptr);
                _pins.push_back(pinptr);
                if (type == "clk" || type.find("C") != string::npos) {
                    type = "CLK";
                }
                IODesign.insert({type, pinptr});
                // IODesignPin.insert({type, pinptr});
            }
            // PinName2Ptr.insert({"IODesignIn", IODesignPin});
        } else if (keyword == "NumOutput") {
            double num;
            iss >> num;
            // map<string, Pin*> IODesignPin;
            for (int i = 0; i < num; i++) {
                getline(file, line);
                istringstream iss(line);
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                Pin* pinptr = new Pin();
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
        } else if (keyword == "FlipFlop") {
            double bitCount, width, height, pinCount;
            string id;

            iss >> bitCount >> id >> width >> height >> pinCount;
            FFCell* FFcellptr = new FFCell(id, width, height, pinCount, bitCount);

            // ensure the size of _ffLib
            addFFLib(FFcellptr, bitCount);
            string temp, name;
            double x, y;
            bool _check;
            for (int i = 0; i < pinCount; ++i) {
                getline(file, line);
                istringstream iss(line);
                iss >> temp >> name >> x >> y;
                // FFcellptr->AllPinName.push_back(name);
                // if (name.find("Q") != string::npos) {
                //     FFcellptr->setOutput(name, 1);
                //     FFcellptr->isOutPin(name);
                //     FFcellptr->setPinOffset(name, pair<double, double>(x, y));

                // } else if (name.find("C") != string::npos) {
                //     FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                // } else {
                //     FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                // }
                _check = (name[1] == 'Q') ? true : false;
                FFcellptr->setPin(name, make_pair(x, y), _check);
                if (name.substr(0, 3) == "CLK")
                    FFcellptr->setClkPin(i);
            }
            // cout << "FF" << " " << bitCount << " " << id << " " << width << " " << height << endl;
        } else if (keyword == "Gate") {
            double width, height;
            int pinCount = 0;
            string id;

            iss >> id >> width >> height >> pinCount;
            BaseCell* bptr = new BaseCell(id, width, height, pinCount);
            addCellLib(bptr);
            CellType2Ptr.insert({id, bptr});
            string temp, name;
            double x, y;
            bool _check;
            for (int i = 0; i < pinCount; i++) {
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
            // cout << "Gate" << " " << id << " " << width << " " << height << endl;
        } else if (keyword == "NumInstances") {
            // Handle Instances
            int numInst;
            iss >> numInst;
            string temp, name, type;
            double x, y;
            CellType* _type;
            int PinOfMnum;
            Module* currentM;
            for (int i = 0; i < numInst; ++i) {
                getline(file, line);
                istringstream instIss(line);
                // Comment: You can put the variables temp, name, type to outside of the while that you can reuse them without re-declarate them -> save some runtime
                instIss >> temp >> name >> type >> x >> y;
                // cout << "testhere" << name << endl;
                // auto it = CellType2Ptr.find(type);
                // if (type.find("G") != string::npos) {
                // cout << "this is Gate type" << endl;
                auto it = CellType2Ptr.find(type);
                map<string, Pin*> PinOfM;
                if (it == CellType2Ptr.end()) {
                    // can't find Standard Cell
                    // Comment: Nice idea for checking error of input file
                    // cout << "Test error" << type << endl;
                } else {
                    _type = CellType2Ptr[type];
                    PinOfMnum = _type->pinNum();
                    currentM = new Module(name, _type, x, y);
                    ModuleName2Ptr[name] = currentM;
                    addModule(currentM);
                    if (_type->isFF())
                        addFF(currentM);
                    for (int i = 0; i < PinOfMnum; ++i) {
                        string PinName = _type->pinName(i);
                        Pin* pinptr = new Pin();
                        pinptr->setPosition(x, y);
                        pinptr->setPinName(PinName);
                        pinptr->setOffset(_type->pinOffsetX(i), _type->pinOffsetY(i));
                        pinptr->setModulePtr(currentM);

                        addPin(pinptr);
                        currentM->addPin(pinptr);
                    }
                }
            }
        } else if (keyword == "NumNets") {
            int num, PinNum;
            string temp, Netname;
            iss >> num;

            string type, FFname, TargetPin;
            Module* _tModule;
            CellType* _type;
            Pin* _tPin;
            Net* netptr;
            bool Isclk;

            for (int i = 0; i < num; i++) {
                Isclk = false;  // 定義這個Net是clkNet!!!每一條定義一個
                getline(file, line);
                istringstream iss(line);

                iss >> temp >> Netname >> PinNum;
                netptr = new Net();
                netptr->setName(Netname);

                for (int j = 0; j < PinNum; j++) {
                    getline(file, line);
                    istringstream piniss(line);
                    piniss >> temp >> type;
                    auto pos = type.find("/");
                    if (pos == type.npos) {
                        // 如果沒有'/'的Net pin角 代表會再IODesign  this is design pin
                        auto it = IODesign.find(type);

                        if (it == IODesign.end()) {
                            // IODesignPin腳 處理跟FF不一樣
                            cout << "No match pin!!" << endl;
                        } else {
                            if (type.find("CLK") != string::npos) {
                                // 找到clk
                                Isclk = true;
                                netptr->setclkFlag(true);
                            }
                            it->second->setNetPtr(netptr);
                            netptr->addPin(it->second);
                        }
                    } else  // 有'/'切割的Net Pin
                    {
                        // cout << "有'/'切割的Net Pin" << endl;
                        FFname = type.substr(0, pos);
                        TargetPin = type.substr(pos + 1);
                        auto it = ModuleName2Ptr.find(FFname);
                        if (it == ModuleName2Ptr.end()) {
                            cout << "can't find correspoding module" << endl;
                        } else {
                            // Found Pin
                            _type = it->second->cellType();
                            if ((TargetPin.substr(0, 3) == "OUT") || (TargetPin[0] == 'Q')) {
                                netptr->setOutputPins(j);
                            }
                            _tModule = it->second;
                            _tPin = _tModule->pin(_type->getPinIdxFromName(TargetPin));
                            _tPin->setNetPtr(netptr);
                            netptr->addPin(_tPin);
                        }
                    }
                }

                // 來保證每個 net 只被加入一次
                // if (Isclk == 1) {
                //     addNet(netptr);
                // } else {
                //     addClkNet(netptr);
                // }
                addNet(netptr);
                if (Isclk)
                    addClkNet(netptr);
            }
        } else if (keyword == "BinWidth" || keyword == "BinHeight" || keyword == "BinMaxUtil") {
            int data;
            iss >> data;
            if (keyword == "BinWidth") {
                setBinWidth(data);
            } else if (keyword == "BinHeight") {
                setBinHeight(data);
            } else if (keyword == "BinMaxUtil") {
                setBinUtil(data);
            }
            initialBinArray();
        } else if (keyword == "PlacementRows") {
            int startX, startY, siteSpacing, siteHeight, totalNumOfSites;
            iss >> startX >> startY >> siteSpacing >> siteHeight >> totalNumOfSites;
            for (auto& rows : _rows) {
                rows->setHeight(siteHeight);
                rows->setSiteSpacing(siteSpacing);
                rows->setNumSites(totalNumOfSites);
                rows->setPosition(startX, startY);
            }
        } else if (keyword == "DisplacementDelay") {
            double delay;
            iss >> delay;
            setDisplacementDelay(delay);

        } else if (keyword == "QpinDelay") {
            string type;
            double delay;
            iss >> type >> delay;
            CellType* _type = CellType2Ptr[type];
            _type->setQdelay(delay);
        } else if (keyword == "TimingSlack") {
            string name, Dpin;
            double slack;
            iss >> name >> Dpin >> slack;
            Module* _tModule = ModuleName2Ptr[name];
            CellType* _type = _tModule->cellType();
            _tModule->pin(_type->getPinIdxFromName(Dpin))->getSlackInfor().setSlack(slack);
        } else if (keyword == "GatePower") {
            string type;
            double power;
            iss >> type >> power;
            BaseCell* _type = CellType2Ptr[type];
            _type->setQdelay(power);
        }
    }
    file.close();
}
void Database::initialBinArray() {
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
    for (int i = 0; i < _numBinCol; ++i) {
        _bins[i].resize(_numBinRow);
        _tempLeft = _xPos + _binWidth * i;
        for (int j = 0; j < _numBinRow; ++j) {
            _tempBottom = _yPos + _binHeight * j;
            _bins[i][j] = new Bin(_tempLeft, _tempBottom, _tempLeft + _binWidth,
                                  _tempBottom + _binHeight, _binArea);
        }
    }
}

void Database::resetBin() {
    for (size_t i = 0; i < _numBinCol; ++i) {
        for (size_t j = 0; j < _numBinRow; ++j) {
            _bins[i][j]->resetBin();
        }
    }
}

void Database::updateBinUtil() {
    Module* _tmpModule;
    double _tmpCenterX, _tmpCenterY;
    int _lbinIdx, _bbinIdx, _rbinIdx, _tbinIdx;
    // Reset bin util to 0
    resetBin();
    // Update bin util
    for (size_t i = 0; i < _numModules; ++i) {
        _tmpModule = module(i);

        _lbinIdx = _tmpModule->x() / _binWidth;
        _bbinIdx = _tmpModule->y() / _binHeight;
        _rbinIdx = (_tmpModule->x() + _tmpModule->width()) / _binWidth;
        _tbinIdx = (_tmpModule->y() + _tmpModule->height()) / _binHeight;

        _rbinIdx = ((_rbinIdx + 1) >= _numBinCol) ? _numBinCol : (_rbinIdx + 1);
        _tbinIdx = ((_tbinIdx + 1) >= _numBinRow) ? _numBinRow : (_tbinIdx + 1);
        _lbinIdx = (_lbinIdx < 0) ? 0 : _lbinIdx;
        _bbinIdx = (_bbinIdx < 0) ? 0 : _bbinIdx;

        for (int j = _lbinIdx; j < _rbinIdx; ++j) {
            for (int k = _bbinIdx; k < _tbinIdx; ++k) {
                _bins[j][k]->updateOverlapArea(Rectangle::overlapArea(
                    _tmpModule->rectangle(), *_bins[j][k]));
            }
        }
    }
}
