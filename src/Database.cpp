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
      _numNets(0),
      _numInput(-1),
      _numOutput(-1),
      _numBinCol(0),
      _numBinRow(0),
      _binWidth(-1),
      _binHeight(-1),
      _binMaxUtil(-1),
      _QpinDelay(-1),
      _alpha(-1),
      _beta(-1),
      _gamma(-1),
      _lambda(-1) {}

void Database::parser(const string& filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        string keyword;
        iss >> keyword;
        if (keyword == "Alpha" || keyword == "Beta" || keyword == "Gamma" || keyword == "Lambda") {
            double alpha, beta, gamma, lambda;
            iss >> alpha >> beta >> gamma >> lambda;
            setalpha(alpha);
            setbeta(beta);
            setgamma(gamma);
            setlambda(lambda);

        } 
        else if (keyword == "DieSize") {
            int left, bottom, right, top;
            iss >> left >> bottom >> right >> top;
            setBoundaryLeft(left);
            setBoundaryBottom(bottom);
            setBoundaryRight(right);
            setBoundaryTop(top);
        } 
        else if (keyword == "NumInput") {
            double num;
            iss >> num;
            map<string, Pin*> IODesignPin;
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
                if (type == "clk") {
                    cout << "HEE" << endl;
                    type = "CLK";
                }
                IODesign.insert({type, pinptr});
                IODesignPin.insert({type, pinptr});
            }
            PinName2Ptr.insert({"IODesignIn", IODesignPin});
        } 
        else if (keyword == "NumOutput") {
            double num;
            iss >> num;
            map<string, Pin*> IODesignPin;
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
                IODesignPin.insert({type, pinptr});
            }
            PinName2Ptr.insert({"IODesignOut", IODesignPin});
        } 
        else if (keyword == "FlipFlop") {
            int bitCount, width, height, pinCount;
            string id;
            string test;
            string test1;

            iss >> bitCount >> id >> width >> height >> pinCount;
            FFCell* FFcellptr = new FFCell(id, width, height, pinCount);

            // ensure the size of _ffLib
            if (_ffLib.size() <= bitCount) {
                _ffLib.resize(bitCount + 1);
            }
            this->_ffLib[bitCount].push_back(FFcellptr);
            this->CellType2Ptr.insert({id, FFcellptr});
            for (int i = 0; i < pinCount; i++) {
                getline(file, line);
                istringstream iss(line);
                string temp, name;
                double x, y;
                iss >> temp >> name >> x >> y;
                FFcellptr->AllPinName.push_back(name);
                if (name.find("Q") != string::npos) {
                    FFcellptr->setOutput(name, 1);
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));

                } else if (name == "CLK") {
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                } else {
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                }
            }

            cout << "FF" << " " << bitCount << " " << id << " " << width << " "
                 << height << endl;
        }

        else if (keyword == "NumInstances") {
            // Handle Instances
            int numInst;
            iss >> numInst;
            for (int i = 0; i < numInst; i++) {
                getline(file, line);
                istringstream instIss(line);
                string temp, name, type;
                double x, y;
                instIss >> temp >> name >> type >> x >> y;
                auto it = CellType2Ptr.find(type);
                int PinOfMnum = it->second->getPinNum();
                map<string, Pin*> PinOfM;
                if (it == CellType2Ptr.end())  // can't find Standard Cell
                {
                    cout << "Test error 0907120" << type << endl;
                } else {
                    Module* currentM = new Module(name, it->second, x, y);
                    ModuleName2Ptr.insert({name, currentM});
                    _modules.push_back(currentM);
                    for (int i = 0; i < PinOfMnum; i++) {
                        string PinName = it->second->AllPinName[i];

                        Pin* pinptr = new Pin();
                        pinptr->setPosition(x, y);
                        pinptr->setPinName(PinName);
                        pair<double, double> tempOffset =
                            it->second->getPinOffset(PinName);
                        pinptr->setOffset(tempOffset.first, tempOffset.second);
                        pinptr->setModulePtr(currentM);
                        PinOfM.insert({PinName, pinptr});
                        _pins.push_back(pinptr);
                    }
                    PinName2Ptr.insert({name, PinOfM});
                }
            }
        }

        else if (keyword == "NumNets") {
            int num, PinNum;
            string temp, Netname;
            iss >> num;
            for (int i = 0; i < num; i++) {
                getline(file, line);
                istringstream iss(line);
                bool clkFlag = 0;
                string type, FFname, TargetPin;
                iss >> temp >> Netname >> PinNum;
                Net* netptr = new Net();
                netptr->setName(Netname);
                this->_nets.push_back(netptr);
                if (Netname == "clk") {
                    netptr->setclkFlag(1);
                    this->Netname2Ptr.insert({Netname, netptr});
                } else if (Netname == "out") {
                    netptr->setOutputPins(1);
                    this->Netname2Ptr.insert({Netname, netptr});
                }

                for (int j = 0; j < PinNum; j++) {
                    getline(file, line);
                    istringstream iss(line);
                    iss >> temp >> type;
                    auto pos = type.find("/");
                    if (pos == type.npos)  // 如果沒有'/'的Net pin角
                    {
                        auto it = IODesign.find(type);
                        if (it == IODesign.end()) {
                            /*cout << "催謀" << endl;*/
                        } else {
                            netptr->addPin(it->second);
                            it->second->setNetPtr(netptr);
                        }

                    } else  // 有'/'切割的Net Pin角
                    {
                        FFname = type.substr(0, pos);
                        TargetPin = type.substr(pos + 1);
                        auto it = ModuleName2Ptr.find(FFname);
                        if (it == ModuleName2Ptr.end()) {
                            cout << "真的崔謀" << endl;
                        } else {
                            auto it = PinName2Ptr.find(FFname);
                            auto PinOfM = it->second.find(TargetPin);
                            PinOfM->second->setNetPtr(netptr);
                            netptr->addPin(PinOfM->second);
                        }
                    }
                }
            }

        }

        else if (keyword == "BinWidth" || keyword == "BinHeight" ||
                 keyword == "BinMaxUtil") {
            int width, height, maxUtil;
            iss >> width >> height >> maxUtil;
            setBinWidth(width);
            setBinHeight(height);
            setBinUtil(maxUtil);

        } else if (keyword == "PlacementRows") {
            int startX, startY, siteSpacing, siteHeight, totalNumOfSites;
            iss >> startX >> startY >> siteSpacing >> siteHeight >>
                totalNumOfSites;
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
            for (auto& row : _ffLib) {
                for (auto& cell : row) {
                    if (cell->getName() == type) {
                        cell->setQdelay(delay);
                    }
                }
            }
            //// Delete memory
            // for (auto& row : _ffLib) {
            //     for (auto& cell : row) {
            //         delete cell;
            //     }
            // }
        } else if (keyword == "TimingSlack") {
            string name, Dpin;
            double timing;
            iss >> name >> Dpin >> timing;
            // for (auto& row : _ffLib) {
            //     for (auto& cell : row) {
            //         if (cell->getName() == name) {
            //             cell->setPower(timing);
            //         }
            //     }
            // }
        } else if (keyword == "GatePower") {
            string type;
            double power;
            iss >> type >> power;
            for (auto& row : _ffLib) {
                for (auto& cell : row) {
                    if (cell->getName() == type) {
                        cell->setPower(power);
                    }
                }
            }
            //// Delete memory
            // for (auto& row : _ffLib) {
            //     for (auto& cell : row) {
            //         delete cell;
            //     }
            // }
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
    int _rowIdx, _colIdx;
    int _lbinIdx, _bbinIdx, _rbinIdx, _tbinIdx;
    double _cellArea;
    double _Ox, _Oy;
    double _cogDistX, _cogDistY;
    // Reset bin util to 0
    resetBin();
    // Update bin util
    for (size_t i = 0; i < _numModules; ++i) {
        _tmpModule = module(i);
        _tmpCenterX = _tmpModule->centerX();
        _tmpCenterY = _tmpModule->centerY();

        _colIdx = (_tmpCenterX - _dieRectangle.left()) / _binWidth;
        _rowIdx = (_tmpCenterY - _dieRectangle.bottom()) / _binHeight;

        _rbinIdx = ((_colIdx + 3) >= _numBinCol) ? _numBinCol : (_colIdx + 3);
        _tbinIdx = ((_rowIdx + 3) >= _numBinRow) ? _numBinRow : (_rowIdx + 3);
        _lbinIdx = ((_colIdx - 3) < 0) ? 0 : (_colIdx - 3);
        _bbinIdx = ((_rowIdx - 3) < 0) ? 0 : (_rowIdx - 3);
    }
}
