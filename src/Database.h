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
#include "Placement.h"
#include "rhombus.h"
using namespace std;

class Database
{
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
    void updateRectangle()
    {
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
    void erasePin(unsigned pinId) { _pins.erase(_pins.begin() + pinId); }
    void eraseModule(unsigned moduleId) { _modules.erase(_modules.begin() + moduleId); }
    void eraseFF(unsigned ffId) { _ffModules.erase(_ffModules.begin() + ffId); }

    // Bin operation
    void initialBinArray();
    void resetBin();
    void updateBinUtil();

    // get design property
    Module *module(unsigned moduleId) { return _modules[moduleId]; }
    Module *ff(unsigned ffId) { return _ffModules[ffId]; }
    Net *net(unsigned netId) { return _nets[netId]; }
    Pin *pin(unsigned pinId) { return _pins[pinId]; }
    Row *row(unsigned rowId) { return _rows[rowId]; }
    Bin *bin(unsigned colIdx, unsigned rowIdx) { return _bins[colIdx][rowIdx]; }

    Pin *input(unsigned inId)
    {
        assert(inId < _numInput);
        return _pins[inId];
    }

    Pin *output(unsigned outId)
    {
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
    unsigned getMaxBitFFLib() const { return _ffLib.end()->first; }
    NetList getClkNets() const { return _clkNets; }

    double getBoundaryTop() const { return _boundaryTop; }
    double getBoundaryLeft() const { return _boundaryLeft; }
    double getBoundaryBottom() const { return _boundaryBottom; }
    double getBoundaryRight() const { return _boundaryRight; }
    FFCell *ffLib(unsigned bitNum, unsigned idx)
    {
        auto it = _ffLib.find(bitNum);
        if (it != _ffLib.end())
        {
            return _ffLib[bitNum][idx];
        }
        else
        {
            cout << "Error: FF library not found!" << endl;
            exit(1);
        }
    }
    unsigned getNumfflibBit(unsigned bit)
    {
        auto it = _ffLib.find(bit);
        if (it != _ffLib.end())
        {
            return _ffLib[bit].size();
        }
        else
        {
            cout << "Error: FF library not found!" << endl;
            exit(1);
        }
    }
    // For slack update
    void sortClkNet();
    void updateSlackAll();
    void updateSlack(Pin *);
    void resetVisit();

    void unMarkedDPin(); // unmarked all clk pin of FF
    // void updateRadius(FFCell *);
    void updateRadius();
    void debankFF();
    Pin *FindPrePin(Pin *inputPin);
    void updateInitialSlackInfo();
    void printResult();

    double getTNS() const;
    unsigned getDen(double) const;
    double totalCost(double) const;

    FFCell *getFFlib(int bit){return _ffLib[bit][0];}

private:
    string _name; // Design Name

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
    int _numBinCol; // Like x index
    int _numBinRow; // Like y index

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
    // vector<History> _pinHistory;

    // void createPinforModule(Module *);
    // void updateRadiusRecur(FFCell*, Module*);
    Module *FindPrePin(Module *currentM);
    
    // void updateInitialSlackInfo();
};

#endif // DATABASE_H