#ifndef MY_SQUARE_H
#define MY_SQUARE_H

#include <iostream>
#include <vector>
#include "point.h"

class Square{
    public:
        Point top_left,top_right,down_left,down_right;

    Square(Point tl,Point tr,Point dl,Point dr): top_left(tl),top_right(tr),down_left(dl),down_right(dr) {};

    bool operator== (const Square& other){
        return (top_left == other.top_left && top_right==other.top_right && 
        down_left==other.down_left && down_right==other.down_right);
    }

    std::vector<double> list(){
        std::vector<double> points;

        std::vector<double> ptl = top_left.list();
        points.insert(points.end(),ptl.begin(),ptl.end());

        std::vector<double> ptr = top_right.list();
        points.insert(points.end(),ptr.begin(),ptr.end());

        std::vector<double> pdl = down_left.list();
        points.insert(points.end(),pdl.begin(),pdl.end());

        std::vector<double> pdr = down_right.list();
        points.insert(points.end(),pdr.begin(),pdr.end());

        return points;
    }
};

#endif