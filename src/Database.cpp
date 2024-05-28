#include "Database.h"

Database::Database()
		: _name(), _boundaryTop(-1), _boundaryBottom(-1), _boundaryLeft(-1), _boundaryRight(-1), _numModules(0),
		  _numNets(0), _numInput(-1), _numOutput(-1), _numBinCol(0), _numBinRow(0), _binWidth(-1), _binHeight(-1), _binMaxUtil(-1),
		  _deltaDelay(-1), _alpha(-1), _beta(-1), _gamma(-1), _lambda(-1) {

}

void Database::parser() {
	//TODO:
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
