#ifndef ROHMBUS_H
#define ROHMBUS_H

#include <cmath>
#include <numbers>
#include <utility>

#include "Point.h"
using namespace std;

class rohmbus {
   public:
    rohmbus() {}
    rohmbus(double _x, double _y, double _rad) {
        _peak[0] = Point2<double>(_x, _y + _rad / 2);
        _peak[1] = Point2<double>(_x + _rad / 2, _y);
        _peak[2] = Point2<double>(_x, _y - _rad / 2);
        _peak[3] = Point2<double>(_x - _rad / 2, _y);
    }
    ~rohmbus() {}

    void RotatePeak(double _deg) {
        double cosine = cos(_deg / 180 * numbers::pi);
        double sine = sin(_deg / 180 * numbers::pi);

        Point2<double> _tempPeak;
        for (size_t i = 0; i < 4; ++i) {
            _tempPeak = _peak[i];
            _peak[i].x = _tempPeak.x * cosine - _tempPeak.y * sine;
            _peak[i].y = _tempPeak.x * sine + _tempPeak.y * cosine;
        }
    }

    Point2<double>& peak(unsigned idx) { return _peak[idx]; }

    static pair<double, double> findCentroidIntersect(rohmbus _rohm0,
                                                      rohmbus _rohm1) {
        // Rotate 45 deg
        _rohm0.RotatePeak(45);
        _rohm1.RotatePeak(45);
        // After rotation:
        // 0: top_left, 1: top_right, 2: bot_right, 3: bot_left
        double _leftBound = max(_rohm0.peak(0).x, _rohm1.peak(0).x);
        double _rightBound = min(_rohm0.peak(1).x, _rohm1.peak(1).x);
        double _botBound = max(_rohm0.peak(2).y, _rohm1.peak(2).y);
        double _topBound = min(_rohm0.peak(0).y, _rohm1.peak(0).y);

        _rohm0.RotatePeak(-45);
        _rohm1.RotatePeak(-45);
        return make_pair((_leftBound + _rightBound) / 2,
                         (_botBound + _topBound) / 2);
    }

   private:
    Point2<double> _peak[4];
    // Before rotate:
    //  0: top, 1: right, 2: bot, 3: left
};
#endif