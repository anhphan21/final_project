#ifndef ROW_H
#define ROW_H

#include <vector>

#include "DatabaseDef.h"
using namespace std;

class Row {
   public:
    enum Orient {
        OR_N,
        OR_W,
        OR_S,
        OR_E,
        OR_FN,
        OR_FW,
        OR_FS,
        OR_FE
    };

    Row() : _x(-1), _y(-1), _siteWidth(-1), _siteHeight(-1), _siteSpacing(-1), _numSites(0), _orient(OR_N), _isSymmetric(1) {}

    Row(double &x, double &y, double siteWidth, double siteHeight, double &set_step) : _x(x), _y(y), _siteWidth(siteWidth), _siteHeight(siteHeight), _siteSpacing(0), _orient(OR_N), _numSites(set_step), _isSymmetric(1) {}

    ////////////////////////////////////////////
    static bool Lesser(const Row &r1, const Row &r2) {
        return (r1._y < r2._y);
    }

    static bool Greater(const Row &r1, const Row &r2) {
        return (r1._y > r2._y);
    }
    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    double x() const { return _x; }
    double y() const { return _y; }
    double height() const { return _siteHeight; }
    double width() const { return _numSites * _siteWidth; }
    double siteSpacing() const { return _siteSpacing; }
    double numSites() const { return _numSites; }

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setPosition(double x, double y) {
        _x = x;
        _y = y;
    }

    void setHeight(double height) { _siteHeight = height; }
    void setSiteSpacing(double siteSpacing) { _siteSpacing = siteSpacing; }
    void setNumSites(unsigned numSites) { _numSites = numSites; }
    void setOrient(Orient orient) { _orient = orient; }
    void setIsSymmetric(bool isSymmetric) { _isSymmetric = isSymmetric; }

   private:
    // variables from benchmark input
    double _x, _y;  // low x and low y
    double _siteWidth;
    double _siteHeight;   // height of row
    double _siteSpacing;  // distance between the beginnings of neighboring sites
    unsigned _numSites;   // number of sites
    Orient _orient;       // orient
    bool _isSymmetric;    // symmetry
};

#endif  // ROW_H