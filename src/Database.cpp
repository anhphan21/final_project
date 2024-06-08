#include "Database.h"

#include <fstream>
#include <sstream>

#include "Pin.h"
#include "Row.h"

using namespace std;

Database::Database()
    : _name(""),
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

// Note for Lily: I think there is something wrong with the pin creation for the module and net so I write this function
// after you create an empty module in NumInstances part of the parser, you can call this function to ccreate the pin of module
void Database::createPinforModule(Module* _tmpModule) {
    BaseCell* cellType = _tmpModule->cellType();
    Pin* _tPin;
    for (size_t i = 0, endi = cellType->pinNum(); i < endi; ++i) {
        _tPin = new Pin(cellType->pinName(i), _tmpModule, 
                        _tmpModule->x() + cellType->pinOffsetX(i), cellType->pinOffsetX(i), 
                        _tmpModule->y() + cellType->pinOffsetY(i), cellType->pinOffsetY(i));
        _tmpModule->addPin(_tPin);
        addPin(_tPin);
    }
}

void Database::parser(const string& filename) {
        ifstream file(filename);
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        string keyword;
        iss >> keyword;
        if (keyword == "Alpha" || keyword == "Beta" || keyword == "Gamma" || keyword == "Lambda") 
        {
            double alpha, beta, gamma, lambda;
            iss >> alpha >> beta >> gamma >> lambda;
            setalpha(alpha);
            setbeta(beta);
            setgamma(gamma);
            setlambda(lambda);

        }
        else if (keyword == "DieSize") 
        {
            int left, bottom, right, top;
            iss >> left >> bottom >> right >> top;
            boundaryLeft(left);
            boundaryBottom(bottom);
            boundaryRight(right);
            boundaryTop(top);

        }
        else if (keyword == "NumInput") 
        {
            double num;
            iss >> num;
            map<string, Pin*> IODesignPin;
            for (int i = 0; i < num; i++) 
            {
                getline(file, line);
                istringstream iss(line);
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                Pin* pinptr = new Pin();
                //input(num)->name(type);
                //input(num)->setPosition(x, y);
                
                pinptr->setPinName(type);
                pinptr->setPosition(x, y);
                pinptr->setModulePtr(nullptr);
                _pins.push_back(pinptr);
                if (type == "clk" ||type.find("C") != string::npos)
                {
                    //cout << "HEE" << endl;
                    type = "CLK";
                }
                IODesign.insert({ type, pinptr });
                IODesignPin.insert({ type , pinptr });
                                
            }
            PinName2Ptr.insert({ "IODesignIn",IODesignPin });
        }
        else if (keyword == "NumOutput") 
        {
            double num;
            iss >> num;
            map<string, Pin*> IODesignPin;
            for (int i = 0; i < num; i++)
            {
                getline(file, line);
                istringstream iss(line);
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                Pin* pinptr = new Pin();
                //output(num)->name(type);
                //output(num)->setPosition(x, y);
                IODesign.insert({ type,pinptr });
                pinptr->setPinName(type);
                pinptr->setPosition(x, y);
                pinptr->setModulePtr(nullptr);
                _pins.push_back(pinptr);
                IODesignPin.insert({ type , pinptr });
            }
            PinName2Ptr.insert({ "IODesignOut",IODesignPin });
        }
        else if (keyword == "FlipFlop")
        {
            double bitCount, width, height, pinCount;
            string id;
    
            iss >> bitCount >> id >> width >> height >> pinCount;
            //cout << "FF ferf" << width <<pinCount << endl;
            FFCell* FFcellptr = new FFCell(id, width, height, pinCount);
            
            // ensure the size of _ffLib
            if (_ffLib.size() <= bitCount) {
                _ffLib.resize(bitCount + 1);
            }
            this->_ffLib[bitCount].push_back(FFcellptr);
            this->CellType2Ptr.insert({ id,FFcellptr });
            //cout << "F PinNum" << FFcellptr->getPinNum() << endl;
            for (int i = 0; i < pinCount; i++) 
            {
                getline(file, line);
                istringstream iss(line);
                string temp, name;
                double  x, y;
                iss >> temp >> name >> x >> y;
                FFcellptr->AllPinName.push_back(name);
                if (name.find("Q") != string::npos)
                {

                    FFcellptr->setOutput(name, 1);
                    //FFcellptr->isOutPin(name);
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
            BaseCell* bptr = new BaseCell();
            this->_cellLib.push_back(bptr);
            this->CellType2Ptr.insert({ id,bptr });
            for (int i = 0; i < pinCount; i++)
            {
                getline(file, line);
                istringstream iss(line);
                string temp, name;
                double  x, y;
                iss >> temp >> name >> x >> y;
                bptr->AllPinName.push_back(name);
                if (name.find("O") != string::npos)
                {

                    bptr->setOutput(name, 1);
                    bptr->setPinOffset(name, pair<double, double>(x, y));

                }
                else
                {
                    bptr->setPinOffset(name, pair<double, double>(x, y));
                }

            }

            //cout << "Gate" << " " << id << " " << width << " " << height << endl;

        }
        else if (keyword == "NumInstances") 
        {
            // Handle Instances
            int numInst;
            iss >> numInst;
            for (int i = 0; i < numInst; i++) {
                getline(file, line); 
                istringstream instIss(line);
                string temp, name, type;
                double x, y;
                instIss >> temp >> name >> type >> x >> y;
                //cout << "testhere" << name << endl;
                //auto it = CellType2Ptr.find(type);
                int outputNum = 0;
                int inputNum = 0;
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
                            
                            bool ISOUTPUT = this->CellType2Ptr[name]->isOutPin(PinName);
                            if (ISOUTPUT)
                            {
                                currentM->addOutPin(pinptr);
                                outputNum++;
                            }
                            else
                            {
                                currentM->addInPin(pinptr);
                                inputNum++;
                            }

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
                        currentM->setNumPins(inputNum, outputNum);
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
                            //cout << type <<"==================" << endl;
                            bool ISOUTPUT = this->CellType2Ptr[type]->isOutPin(PinName);
                            if (ISOUTPUT)
                            {
                                currentM->addOutPin(pinptr);
                                outputNum++;
                            }
                            else
                            {
                                currentM->addInPin(pinptr);
                                inputNum++;
                            }
                            pinptr->setPosition(x, y);
                            pinptr->setPinName(PinName);
                            pair<double, double > tempOffset = it->second->getPinOffset(PinName);
                            pinptr->setOffset(tempOffset.first, tempOffset.second);
                            pinptr->setModulePtr(currentM);
                            PinOfM.insert({ PinName,pinptr });
                            _pins.push_back(pinptr);
                            //cout << "�Ҳ�" << name << " " << "PinName" << PinName << endl;
                        }
                        PinName2Ptr.insert({ name ,PinOfM });
                        //cout << name << endl;
                        currentM->setNumPins(inputNum, outputNum);
                    }
                    
                }
            }
        }


        else if (keyword == "NumNets") {
            int num, PinNum;
            string temp, Netname;
            iss >> num;
            for (int i = 0; i < num; i++)
            {
                bool Isclk = 0; //�w�q�o��Net�OclkNet!!!�C�@���w�q�@��
                getline(file, line);
                istringstream iss(line);
                bool clkFlag = 0;
                string type, FFname, TargetPin;
                iss >> temp >> Netname >> PinNum;
                Net* netptr = new Net();
                netptr->setName(Netname);
                //this->_nets.push_back(netptr);
                //cout << "PinNum: " << PinNum << endl;
                
                for (int j = 0; j < PinNum; j++)
                {
                    
                    getline(file, line);
                    istringstream piniss(line);
                    piniss >> temp >> type;
                    //cout << "type: " << type << endl;
                    auto pos = type.find("/");
                    if (pos == type.npos) //�p�G�S��'/'��Net pin�� �N���|�AIODesign
                    {
                        auto it = IODesign.find(type);
                        
                        if (it == IODesign.end()) //IODesignPin�} �B�z��FF���@��
                        {
                            //cout << "�ʿ�" << endl;
                        }
                        else if (type.find("CLK") != string::npos) //���clk
                        {
                            Isclk = 1;
                            netptr->setclkFlag(1);
                            auto it = this->IODesign.find(type);
                            it->second->setNetPtr(netptr);
                            netptr->addPin(it->second);;
                        }
                        /*else if (type == "out" || type.find("O") != string::npos)
                        {
                            cout << "����output" << endl;
                           auto it = this->IODesign.find(type);
            
                           it->second->setNetPtr(netptr);
                           netptr->addPin(it->second);
                        }*/
                        else
                        {
                            //cout << "����input" << endl;
                            auto it = this->IODesign.find(type);
                            it->second->setNetPtr(netptr);
                            netptr->addPin(it->second);
                        }

                    }
                    else  //��'/'���Ϊ�Net Pin
                    {
                        //cout << "��'/'���Ϊ�Net Pin" << endl;
                        FFname = type.substr(0, pos);
                        TargetPin = type.substr(pos + 1);
                        auto it = ModuleName2Ptr.find(FFname);
                        if (it == ModuleName2Ptr.end())
                        {
                            //cout << "can't find" << endl;
                        }
                        else if (TargetPin.find("Q") != string::npos)
                        {
                            auto it = PinName2Ptr.find(FFname);
                            auto PinOfM = it->second.find(TargetPin);
                            PinOfM->second->module()->cellType()->isOutPin(TargetPin);
                            PinOfM->second->setNetPtr(netptr);
                            netptr->addPin(PinOfM->second);
                        }
                        else
                        {
                            auto it = PinName2Ptr.find(FFname);
                            auto PinOfM = it->second.find(TargetPin);
                            //cout << FFname << endl;
                            PinOfM->second->setNetPtr(netptr);// !!�o�̷|�]��end
                            netptr->addPin(PinOfM->second); 
                        }
                    }
                }

                // �ӫO�ҨC�� net �u�Q�[�J�@��
                if (Isclk == 1)
                {
                    addNet(netptr);
                }
                else
                {
                    addClkNet(netptr);
                }

            }
        }



        else if (keyword == "BinWidth" || keyword == "BinHeight" || keyword == "BinMaxUtil") {
            int width, height, maxUtil;
            iss >> width >> height >> maxUtil;
            setBinWidth(width);
            setBinHeight(height);
            setBinUtil(maxUtil);

        }
        else if (keyword == "PlacementRows") {
            int startX, startY, siteSpacing, siteHeight, totalNumOfSites;
            iss >> startX >> startY >> siteSpacing >> siteHeight >> totalNumOfSites;
            for (auto& rows : _rows) {
                rows->setHeight(siteHeight);
                rows->setSiteSpacing(siteSpacing);
                rows->setNumSites(totalNumOfSites);
                rows->setPosition(startX, startY);
                
            }
            


        }
        else if (keyword == "DisplacementDelay") {
            double delay;
            iss >> delay;
            setDisplacementDelay(delay);

        }
        else if (keyword == "QpinDelay") {
            string type;
            double delay;
            iss >> type >> delay;
            for (int i = 0; i < _ffLib.size(); i++)
            {
                for (int j = 0; j < _ffLib[i].size(); j++)
                {
                    if (_ffLib[i][j]->getName() == type)
                    {
                        _ffLib[i][j]->setQdelay(delay);
                    }
                    else
                        break;
                }
            }
        }
        else if (keyword == "TimingSlack") {
            string name, Dpin;
            double slack;
            iss >> name >> Dpin >> slack;
            auto it = PinName2Ptr.find(name);
            auto pin = it->second.find(Dpin);
            pin->second->setSlack(slack);

        }
        else if (keyword == "GatePower") {
            string type;
            double power;
            iss >> type >> power;
            for (int i = 0; i < _ffLib.size(); i++)
            {
                for (int j = 0; j < _ffLib[i].size(); j++)
                {
                    if (_ffLib[i][j]->getName() == type)
                    {
                        _ffLib[i][j]->setPower(power);
                    }
                    else
                        break;
                }
            }
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

void Database::sortClkNet() {
    // TODO:
    for (unsigned i = 0, endi = _clkNets.size(); i < endi; ++i) {
    }
}

void Database::resetVisit() {
    for (unsigned i = 0, endi = _clkNets.size(); i < endi; ++i) {
    }
}