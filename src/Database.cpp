#include "Database.h"
#include <sstream>
#include <fstream>

using namespace std;


Database::Database()
		: _name(), _boundaryTop(-1), _boundaryBottom(-1), _boundaryLeft(-1), _boundaryRight(-1), _numModules(0),
		  _numNets(0), _numInput(-1), _numOutput(-1), _numBinCol(0), _numBinRow(0), _binWidth(-1), _binHeight(-1), _binMaxUtil(-1),
		  _QpinDelay(-1), _alpha(-1), _beta(-1), _gamma(-1), _lambda(-1) {

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
            for (int i = 0; i < num; i++) 
            {
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                // PinID is "unsigned" type?
                //input(num)->setPinId(type);
                //input(num)->setPosition(x, y);
            }
        }
        else if (keyword == "NumOutput") 
        {
            double num;
            iss >> num;
            for (int i = 0; i < num; i++)
            {
                string temp, type;
                double x, y;
                iss >> temp >> type >> x >> y;
                // PinID is "unsigned" type?
                //output(num)->setPinId(type);
                //output(num)->setPosition(x, y);
            }
        }
        else if (keyword == "FlipFlop")
        {
            int bitCount, width, height, pinCount, numPin;
            string id;
            iss >> bitCount >> id >> width >> height >> pinCount >> numPin;
            FFCell* FFcellptr = new FFCell(id, width, height, pinCount);
            this->_ffLib[pinCount].push_back(FFcellptr);
            this->CellType2Ptr.insert({ id,FFcellptr });
            for (int i = 0; i < numPin; i++) 
            {
                string temp, name;
                double  x, y;
                iss >> temp >> name >> x >> y;
                if (name.find("Q") != string::npos)
                {
                    FFcellptr->setOutput(name, 1);
                    FFcellptr->setPinOffset(name, pair<double,double>(x, y));
                }
                else if (name == "CLK")
                {
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                }
                else
                {
                    FFcellptr->setPinOffset(name, pair<double, double>(x, y));
                }

            }

            cout << "FF" << " " << bitCount << " " << id << " " << width << " " << height << endl;

        }


        else if (keyword == "NumInstances") {
            // Handle Instances
            int numInst;
            iss >> numInst;
            for (int i = 0; i < numInst; i++)
            {
                string temp, name, type;
                double x, y;
                iss >> temp >> name >> type >> x >> y;
                //Module* currentM = new Module(name, type, x, y, 0);
                //this->_cellLib.push_back(currentM);

            }
        }


        else if (keyword == "NumNets") {
            int num,PinNum;
            string temp, Netname;
            iss >> num;
            for (int i = 0; i < num; i++)
            {
                iss >> temp >> Netname >> PinNum;
                string type, FFname , TargetPin;
                for (int j = 0; j < PinNum; j++)
                {
                    iss >> temp >> type;
                    auto pos = type.find("/");
                    if (pos == type.npos)
                    {
                        
                    }
                    else
                    {
                        FFname = type.substr(0, pos);
                        TargetPin = type.substr(pos + 1);
                    }
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
            int startX, startY, siteWidth, siteHeight, totalNumOfSites;
            iss >> startX >> startY >> siteWidth >> siteHeight >> totalNumOfSites;
            // for (auto& rows : _rows) {
            //     rows->setHeight(siteHeight);
            //     rows->setWidth(siteWidth);
            //     rows->numsites(totalNumOfSites);
            //     rows->x(startX);
            //     rows->y(startY);
                
            // }

        }
        else if (keyword == "DisplacementDelay") {
            double delay;
            iss >> delay;

        }
        else if (keyword == "QpinDelay") {
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
            // Delete memory
            for (auto& row : _ffLib) {
                for (auto& cell : row) {
                    delete cell;
                }
            }
        }
        else if (keyword == "TimingSlack") {
            string name, Dpin;
            double timing;
            iss >> name >> Dpin >> timing;
            for (auto& row : _ffLib) {
                for (auto& cell : row) {
                    if (cell->getName() == name) {
                        cell->setPower(timing);
                    }
                }
            }
            // Delete memory
            for (auto& row : _ffLib) {
                for (auto& cell : row) {
                    delete cell;
                }
            }
        }
        else if (keyword == "GatePower") {
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
            // Delete memory
            for (auto& row : _ffLib) {
                for (auto& cell : row) {
                    delete cell;
                }
            }
        }
    }

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
	Module *_tmpModule;
	double _tmpCenterX, _tmpCenterY;
	int _rowIdx, _colIdx;
	int _lbinIdx, _bbinIdx, _rbinIdx, _tbinIdx;
	double _cellArea;
	double _Ox, _Oy;
	double _cogDistX, _cogDistY;
	//Reset bin util to 0
	resetBin();
	//Update bin util
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
