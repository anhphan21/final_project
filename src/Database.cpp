#include "Database.h"
#include "Row.h"
#include "Pin.h"
#include <sstream>
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
        if (keyword == "Alpha" || keyword == "Beta" || keyword == "Gamma" || keyword == "Lambda") 
        {
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
        } else if (keyword == "NumInput") {
            iss >> _numInput;
            // map<string, Pin*> IODesignPin;
            for (int i = 0; i < _numInput; ++i) {
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
            iss >> _numOutput;
            // map<string, Pin*> IODesignPin;
            for (int i = 0; i < _numOutput; i++) {
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

            string _clkPin;
            double _clkX, _clkY;

            // ensure the size of _ffLib
            if (_ffLib.size() <= bitCount) {
                _ffLib.resize(bitCount + 1);
            }
            this->_ffLib[bitCount].push_back(FFcellptr);
            this->CellType2Ptr.insert({ id,FFcellptr });
            cout << "F PinNum" << FFcellptr->getPinNum() << endl;
            for (int i = 0; i < pinCount; i++) 
            {
                getline(file, line);
                istringstream iss(line);
                iss >> temp >> name >> x >> y;
                FFcellptr->AllPinName.push_back(name);
                if (name.find("Q") != string::npos)
                {

                    FFcellptr->setOutput(name, 1);
                    FFcellptr->isOutPin(name);
                    FFcellptr->setPinOffset(name, pair<double,double>(x, y));
                    
                }
                else if (name.find("C") != string::npos)
                {
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                }
                else
                {
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                }

            }

            //cout << "FF" << " " << bitCount << " " << id << " " << width << " " << height << endl;

        }

        else if (keyword == "Gate")
        {
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
            iss >> _numModules;
            string temp, name, type;
            double x, y;
            CellType* _type;
            int PinOfMnum;
            Module* currentM;
            for (int i = 0; i < _numModules; ++i) {
                getline(file, line);
                istringstream instIss(line);
                // Comment: You can put the variables temp, name, type to outside of the while that you can reuse them without re-declarate them -> save some runtime
                instIss >> temp >> name >> type >> x >> y;
                //cout << "testhere" << name << endl;
                //auto it = CellType2Ptr.find(type);
                if (type.find("G") != string::npos)
                {
                    //cout << "this is Gate type" << endl;
                    auto it = CellType2Ptr.find(type);
                    int PinOfMnum = it->second->getPinNum();
                    map<string, Pin*> PinOfM;
                    if (it == CellType2Ptr.end()) //can't find Standard Cell
                    {
                        //cout << "Test error" << type << endl;
                    }
                    else
                    {
                        Module* currentM = new Module(name, it->second, x, y);
                        ModuleName2Ptr.insert({ name, currentM });
                        _modules.push_back(currentM);
                        for (int i = 0; i < PinOfMnum; i++)
                        {
                            string PinName = it->second->AllPinName[i];

                            Pin* pinptr = new Pin();
                            pinptr->setPosition(x, y);
                            pinptr->setPinName(PinName);
                            pair<double, double > tempOffset = it->second->getPinOffset(PinName);
                            pinptr->setOffset(tempOffset.first, tempOffset.second);
                            pinptr->setModulePtr(currentM);
                            PinOfM.insert({ PinName,pinptr });
                            _pins.push_back(pinptr);
                        }
                        PinName2Ptr.insert({ name ,PinOfM });
                        //cout <<"su3cl3"<< name << endl;
                    }
                }
                else 
                {
                    //cout <<"test point1"<< type << endl;
                    auto it = CellType2Ptr.find(type);
                    int PinOfMnum = it->second->getPinNum();
                    //cout << "test point2" << PinOfMnum << endl;
                    map<string, Pin*> PinOfM;
                    if (it == CellType2Ptr.end()) //can't find Standard Cell
                    {
                        //cout << "Test error" << type << endl;
                    }
                    else
                    {
                        Module* currentM = new Module(name, it->second, x, y);
                        ModuleName2Ptr.insert({ name, currentM });
                        _modules.push_back(currentM);
                        //cout <<"HEREEE"<< PinOfMnum << endl;
                        for (int i = 0; i < PinOfMnum; i++)
                        {
                            string PinName = it->second->AllPinName[i];

                            Pin* pinptr = new Pin();
                            pinptr->setPosition(x, y);
                            pinptr->setPinName(PinName);
                            pair<double, double > tempOffset = it->second->getPinOffset(PinName);
                            pinptr->setOffset(tempOffset.first, tempOffset.second);
                            pinptr->setModulePtr(currentM);
                            PinOfM.insert({ PinName,pinptr });
                            _pins.push_back(pinptr);
                        }
                        PinName2Ptr.insert({ name ,PinOfM });
                        //cout << name << endl;
                        
                    }
                    
                }
            }
        } else if (keyword == "NumNets") {
            int PinNum;
            string temp, Netname;
            iss >> _numNet;

            string type, FFname, TargetPin;
            Module* _tModule;
            CellType* _type;
            Pin* _tPin;
            Net* netptr;
            bool Isclk;

            for (int i = 0; i < _numNet; i++) {
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
                        
                        if (it == IODesign.end()) //IODesignPin腳 處理跟FF不一樣
                        {
                           //cout << "nothing" << endl; 
                        }
                        else if (type.find("CLK") != string::npos) //找到clk
                        {
                            Isclk = 1;
                            netptr->setclkFlag(1);
                            auto it = this->IODesign.find(type);
                            it->second->setNetPtr(netptr);
                            netptr->addPin(it->second);;
                        }
                        /*else if (type == "out" || type.find("O") != string::npos)
                        {
                            cout << "對應output" << endl;
                           auto it = this->IODesign.find(type);
            
                           it->second->setNetPtr(netptr);
                           netptr->addPin(it->second);
                        }*/
                        else
                        {
                            //cout << "對應input" << endl;
                            auto it = this->IODesign.find(type);
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
            }
            else if(keyword == "BinMaxUtil")
            {
                setBinUtil(data);
            }
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
    initialBinArray();
    file.close();
}
void Database::initialBinArray() {
	//Initial Die Boundary
	assert((_boundaryRight != -1) || (_boundaryLeft != -1) || (_boundaryTop != -1) || (_boundaryBottom != -1));
	//Initial Bin information
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
			_bins[i][j] = new Bin(_tempLeft, _tempBottom, _tempLeft + _binWidth, _tempBottom + _binHeight, _binArea);
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
