#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

#include "Module.h"
#include "Point.h"

using namespace std;

int main() {
    double _centX1, _centX0;
    double _centY1, _centY0;
    double _rad0, _rad1;

    _centX0 = 10;
    _centY0 = 10;
    _rad0 = 6;

    _centX1 = 12;
    _centY1 = 9;
    _rad1 = 4;

    vector<Module*> _mergeList;
    vector<vector<Point2<double>>> _rohmbusList;
    // 0: top, 1: right, 2: bot, 3: left

    vector<Point2<double>> _rohmbusPoint(4);
    double _tRad, _tX, _tY;

    for (size_t i = 0, endi = _mergeList.size(); i < endi; ++i) {
        _tRad = _mergeList[i]->radius();
        _tX = _mergeList[i]->centerX();
        _tY = _mergeList[i]->centerY();

        _rohmbusPoint[0] = Point2<double>(_tX, _tY+_tRad/2);
        _rohmbusPoint[1] = Point2<double>(_tX+_tRad/2, _tY);
        _rohmbusPoint[2] = Point2<double>(_tX, _tY-_tRad/2);
        _rohmbusPoint[3] = Point2<double>(_tX-_tRad/2, _tY);

        // _rohmbusList.(_rohmbusPoint);
    }
    return 0;
}