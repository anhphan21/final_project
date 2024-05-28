#include "Database.h"
#include <sstream>
#include <fstream>

using namespace std;


Database::Database()
		: _name(), _boundaryTop(-1), _boundaryBottom(-1), _boundaryLeft(-1), _boundaryRight(-1), _numModules(0),
		  _numNets(0), _numInput(-1), _numOutput(-1), _numBinCol(0), _numBinRow(0), _binWidth(-1), _binHeight(-1), _binMaxUtil(-1),
		  _deltaDelay(-1), _alpha(-1), _beta(-1), _gamma(-1), _lambda(-1) {

}

void Database::parser(const string& filename) {
	//TODO:
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        string keyword;
        iss >> keyword;
        if (keyword == "Alpha" || keyword == "Beta" || keyword == "Gamma" || keyword == "Lambda") 
        {
            double a, b, g, l;
            iss >> a >> b >> g >> l;
            setalpha(a);
            setbeta(b);
            setgamma(g);
            setlambda(l);

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

            }
        }
        else if (keyword == "FlipFlop")
        {
            int bitCount, width, height, pinCount, numPin;
            string id;
            iss >> bitCount >> id >> width >> height >> pinCount >> numPin;
            for (int i = 0; i < numPin; i++) 
            {
                string type, name, x, y;
                iss >> type >> name >> x >> y;
                if (name.find("Q") != string::npos)
                {
                    //碰到輸出該幹嘛幹嘛
                }
                else if (name == "CLK")
                {

                }
                else
                {
                
                }

            }

            cout << "FF" << " " << bitCount << " " << id << " " << width << " " << height << endl;

        }
        //else if (keyword == "Pin" && currentFF != nullptr) {
        //    string name;
        //    int x, y;
        //    iss >> name >> x >> y;
        //    /*currentPin = new Pin(name, x, y);*/
        //    if (name.find("Q") != string::npos) {
        //        /*currentFF->setOutput(currentPin); */
        //        currentPin->setOffset(x, y);
        //    }
        //    else if (name == "CLK") {
        //        /*currentFF->setClockPin(currentPin);*/
        //        currentPin->setOffset(x, y);
        //    }
        //    else {
        //        /*currentFF->addDataPin(currentPin);*/
        //        currentPin->setOffset(x, y);
        //    }
        //    cout << "Pin" << " " << name << " " << x << " " << y << endl;
        //}
        
        else if (keyword == "NumInstances") {
            // Handle Instances
            int numInst;
            iss >> numInst;
            for (int i = 0; i < numInst; i++)
            {
                string temp, name, type;
                double x, y;
                iss >> temp >> name >> type >> x >> y;

            }
        }
        else if (keyword == "NumNets") {
            int num;
            iss >> num;


        }
        //else if (keyword == "Net") {
        //    int numNet;
        //    string netType, netName;
        //    iss >> netType >> netName >> numNet;
        //}
        //else if (keyword == "Pin" && currentModule != nullptr) {
        //    string p1, p2, slash;
        //    iss >> p1 >> slash >> p2;
        //    // where to save a pair of pin with net

        //}
        else if (keyword == "BinWidth" || keyword == "BinHeight" || keyword == "BinMaxUtil") {
            int width, height, maxUtil;
            iss >> width >> height >> maxUtil;


        }
        else if (keyword == "PlacementRows") {
            int startX, startY, siteWidth, siteHeight, totalNumOfSites;
            iss >> startX >> startY >> siteWidth >> siteHeight >> totalNumOfSites;

        }
        else if (keyword == "DisplacementDelay") {
            double delay;
            iss >> delay;

        }
        else if (keyword == "QpinDelay") {
            string type;
            double delay;
            iss >> type >> delay;

        }
        else if (keyword == "TimingSlack") {
            string name, Dpin;
            double timing;
            iss >> name >> Dpin >> timing;

        }
        else if (keyword == "GatePower") {
            string type;
            double power;
            iss >> type >> power;

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
