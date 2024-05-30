#include "Database.h"

Database::Database()
	: _name(), _boundaryTop(-1), _boundaryBottom(-1), _boundaryLeft(-1), _boundaryRight(-1), _numModules(0),
	  _numNets(0), _numInput(-1), _numOutput(-1), _numBinCol(0), _numBinRow(0), _binWidth(-1), _binHeight(-1),
	  _binMaxUtil(-1),
	  _deltaDelay(-1), _alpha(-1), _beta(-1), _gamma(-1), _lambda(-1)
{
}

void Database::parser()
{
	// TODO:
}

void Database::connectPinsWithModulesAndNets()
{
	unsigned _checkOutIdx;
	Pin *_tmpPin;

	for (unsigned i = 0; i < _numNets; ++i)
	{
		_checkOutIdx = _nets[i]->getOutIdx();
		for (unsigned j = 0, endj = _nets[i]->pinNum(); j < endj; ++j)
		{
			_tmpPin = _nets[i]->pin(j);
			if (j == _checkOutIdx)
				_tmpPin->module()->addOutPin(_tmpPin);
			else
				_tmpPin->module()->addInPin(_tmpPin);
		}
	}
}

void Database::initialBinArray()
{
	// Initial Die Boundary
	assert((_boundaryRight != -1) || (_boundaryLeft != -1) || (_boundaryTop != -1) || (_boundaryBottom != -1));
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
			_bins[i][j] = new Bin(_tempLeft, _tempBottom, _tempLeft + _binWidth, _tempBottom + _binHeight, _binArea);
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
	int _lbinIdx, _bbinIdx, _rbinIdx, _tbinIdx;
	Rectangle _tmpRect;

	// Reset bin util to 0
	resetBin();
	// Update bin util
	for (size_t i = 0; i < _numModules; ++i)
	{
		_tmpModule = module(i);
		_tmpRect = _tmpModule->rectangle();

		_lbinIdx = _tmpModule->x() / _binWidth;
		_rbinIdx = (_tmpModule->x() + _tmpModule->width()) / _binWidth;
		_bbinIdx = _tmpModule->y() / _binHeight;
		_tbinIdx = (_tmpModule->y() + _tmpModule->height()) / _binHeight;

		_lbinIdx = (_lbinIdx < 0) ? 0 : _lbinIdx;
		_rbinIdx = ((_rbinIdx + 1) >= _numBinCol) ? _numBinCol : (_rbinIdx + 1);
		_bbinIdx = (_bbinIdx < 0) ? 0 : _bbinIdx;
		_tbinIdx = ((_tbinIdx + 1) >= _numBinRow) ? _numBinRow : (_tbinIdx + 1);

		for (size_t j = _lbinIdx; j < _rbinIdx; ++j)
		{
			size_t k = _bbinIdx;
			for (; k < _tbinIdx; ++k)
			{
				_bins[j][k]->updateOverlapArea(Rectangle::overlapArea(*_bins[j][k], _tmpRect));
			}
		}
	}
}