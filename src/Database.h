#ifndef DATABASE_H
#define DATABASE_H

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "Bin.h"
#include "DatabaseDef.h"
#include "History.h"
#include "Module.h"
#include "Net.h"
#include "Node.h"
#include "Pin.h"
using namespace std;

class Database {
   public:
    Database();
    ~Database() = default;

    void parser(const string &filename);

    // Design parameters
    void setName(string &name) { _name = name; }

    void setAlpha(double alpha) { _alpha = alpha; }
    void setBeta(double beta) { _beta = beta; }
    void setGamma(double gamma) { _gamma = gamma; }
    void setLambda(double lambda) { _lambda = lambda; }

    // Boundaries and Rectangle
    void setBoundaryTop(double boundaryTop) { _boundaryTop = boundaryTop; }
    void setBoundaryLeft(double boundaryLeft) { _boundaryLeft = boundaryLeft; }
    void setBoundaryBottom(double boundaryBottom) { _boundaryBottom = boundaryBottom; }
    void setBoundaryRight(double boundaryRight) { _boundaryRight = boundaryRight; }
    void updateRectangle() {
        _dieRectangle.setBounds(_boundaryLeft, _boundaryBottom, _boundaryRight, _boundaryTop);
    }

    void setNumInput(unsigned numIn) { _numInput = numIn; }
    void setNumOutput(unsigned numOut) { _numOutput = numOut; }

    void setBinWidth(double w) { _binWidth = w; }
    void setBinHeight(double h) { _binHeight = h; }
    void setBinUtil(double u) { _binMaxUtil = u; }

    void setDisplacementDelay(double delay) { _dDelay = delay; }

    // methods for design (hyper-graph) construction
    void addModule(Module *module) { _modules.push_back(module); }
    void addFF(Module *ff) { _ffModules.push_back(ff); }
    void addNet(Net *net) { _nets.push_back(net); }
    void addClkNet(Net *clk) { _clkNets.push_back(clk); }
    void addPin(Pin *pin) { _pins.push_back(pin); }
    void addRow(Row *row) { _rows.push_back(row); }
    void addCellLib(CellType *cellLib) { _cellLib.push_back(cellLib); }
    void addFFLib(FFCell *ffLib, unsigned bitNum) { _ffLib[bitNum].push_back(ffLib); }

    // Bin operation
    void initialBinArray();
    void resetBin();
    void updateBinUtil();

    // get design property
    Module *module(unsigned moduleId) { return _modules[moduleId]; }
    Net *net(unsigned netId) { return _nets[netId]; }
    Pin *pin(unsigned pinId) { return _pins[pinId]; }
    Row *row(unsigned rowId) { return _rows[rowId]; }
    Bin *bin(unsigned colIdx, unsigned rowIdx) { return _bins[colIdx][rowIdx]; }

    Pin *input(unsigned inId) {
        assert(inId < _numInput);
        return _pins[inId];
    }

    Pin *output(unsigned outId) {
        assert(outId < _numOutput);
        return _pins[_numInput + outId];
    }

    unsigned getNumModules() const { return _modules.size(); }
    unsigned getNumFF() const { return _ffModules.size(); }
    unsigned getNumNets() const { return _nets.size(); }
    unsigned getNumClkNets() const { return _clkNets.size(); }
    unsigned getNumDataNets() const { return getNumNets() - getNumClkNets(); }
    unsigned getNumPins() const { return _pins.size(); }
    unsigned getNumInputs() const { return _numInput; }
    unsigned getNumOutputs() const { return _numOutput; }
    unsigned getNumRows() const { return _rows.size(); }
    double getAlpha() const { return _alpha; }
    double getBeta() const { return _beta; }
    double getGamma() const { return _gamma; }
    double getLambda() const { return _lambda; }
    double getDisplacementDelay() const { return _dDelay; }
    double boundaryLeft() const { return _boundaryLeft; }
    double boundaryTop() const { return _boundaryTop; }
    double boundaryRight() const { return _boundaryRight; }
    double boundaryBot() const { return _boundaryBottom; }

    // For slack update
    void sortClkNet();
    void updateSlackAll();
    void updateSlack(Pin *);
    void resetVisit();

    void unMarkedDPin();  // unmarked all clk pin of FF
    void updateRadius(FFCell *);
    void debankFF();

    void printResult();
    void plotPlacementResult(const string outfilename, bool isPrompt = false);

   private:
    string _name;  // Design Name

    // Design Data
    unsigned _numModules;
    unsigned _numNet;
    unsigned _numInput;
    unsigned _numOutput;

    ModuleList _modules;
    NetList _nets;
    PinList _pins;
    RowList _rows;
    BinList _bins;

    // Library
    FFLLibrary _ffLib;
    CellLibrary _cellLib;

    // Design statics
    Rectangle _dieRectangle;
    double _boundaryTop;
    double _boundaryLeft;
    double _boundaryBottom;
    double _boundaryRight;

    // Bin statics
    double _binWidth;
    double _binHeight;
    double _binMaxUtil;
    int _numBinCol;  // Like x index
    int _numBinRow;  // Like y index

    // For FF merging
    double _dDelay;
    double _alpha;
    double _beta;
    double _gamma;
    double _lambda;

    // Caching
    map<string, BaseCell *> CellType2Ptr;
    map<string, Module *> ModuleName2Ptr;
    map<string, Pin *> IODesign;

    // Caching the list for processing
    ModuleList _ffModules;
    NetList _clkNets;

    // History for output
    vector<History> _pinHistory;

    void createPinforModule(Module *);
    void updateRadiusRecur(FFCell*, Module*);
};

#endif  // DATABASE_H