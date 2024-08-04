#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <algorithm>
#include <cmath>
#include "DatabaseDef.h"
using namespace std;

class Rectangle {
   public:
    Rectangle() : _left(0), _bottom(0), _right(0), _top(0) {
    }

    Rectangle(double left, double bottom, double right, double top)
        : _left(left), _bottom(bottom), _right(right), _top(top) {
    }

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    double left() const { return _left; }
    double bottom() const { return _bottom; }
    double right() const { return _right; }
    double top() const { return _top; }
    double width() const { return _right - _left; }
    double height() const { return _top - _bottom; }
    double centerX() const { return (_left + _right) / 2; }
    double centerY() const { return (_bottom + _top) / 2; }

    double getX1() const { return _x1; }
    double getX2() const { return _x2; }
    double getY1() const { return _y1; }
    double getY2() const { return _y2; }
    double getWidth() const { return _width; }
    double getHeight() const { return _height; }
    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setBounds(double left, double bottom, double right, double top) {
        _left = left;
        _bottom = bottom;
        _right = right;
        _top = top;
    }

    void setInf(double x1, double y1,double x2, double y2)
    {
        _x1 = x1;
        _y1 = y1;
        _x2 = x2;
        _y2 = y2;
        _width = fabs(x2 - x1);
        _height = fabs(y2 - y1);
    }
   
    /////////////////////////////////////////////
    // overlap area of two rectangles
    /////////////////////////////////////////////
    static double overlapArea(const Rectangle &rect1, const Rectangle &rect2) {
        double overlapH = min(rect1.right(), rect2.right()) - max(rect1.left(), rect2.left());
        double overlapV = min(rect1.top(), rect2.top()) - max(rect1.bottom(), rect2.bottom());
        if (overlapH < 0) overlapH = 0;
        if (overlapV < 0) overlapV = 0;
        return overlapH * overlapV;
    }

   private:
    double _left;
    double _bottom;
    double _right;
    double _top;
    
    //feasible region of intersection
    double _x1, _y1;//left bottom
    double _x2, _y2;//right top
    double _width, _height;  // width and height of rectangle
};

#endif  // RECTANGLE_H
