#ifndef BIN_H
#define BIN_H

#include <vector>

#include "Rectangle.h"
// #include "Point.h"
#include "DatabaseDef.h"
using namespace std;

class Bin : public Rectangle
{
public:
	Bin() : Rectangle(), _overlapArea(0), _binArea(0)
	{
	}

	Bin(double left, double bottom, double right, double top, double binArea) : _overlapArea(0), _binArea(binArea)
	{
		setBounds(left, bottom, right, top);
	}

	void resetBin()
	{
		_overlapArea = 0;
	}

	double getOverlapArea() const { return _overlapArea; }
	void updateOverlapArea(double overlapArea) { _overlapArea += overlapArea; }

	double getDensity() const { return _overlapArea / _binArea; }

private:
	double _overlapArea; // cal in Rectangle.h
	double _binArea;
};

#endif // BIN_H
