#ifndef MY_SQUARE_H
#define MY_SQUARE_H

#include <iostream>
#include <vector>
#include "point.h"

class Square{
    public:
    Point top_left,top_right,down_left,down_right,center_p;

    Square(Point tl,Point tr,Point dl,Point dr): top_left(tl),top_right(tr),down_left(dl),down_right(dr) {
        double w = top_right.x - top_left.x;
        double h = down_left.y -top_left.y;
        center_p = Point(top_left.x + (w/2),top_left.y + (h/2));
    };
    Square(Point center,double width,double height){
        // height is in case coordinates needs deforming to make a square (such as centered coordinates on rectangle).
        center_p = center;
        top_left =  Point(center.x-(width/2), center.y-(height/2));
        top_right = Point(center.x+(width/2), center.y-(height/2));
        down_left = Point(center.x-(width/2), center.y+(height/2));
        down_right = Point(center.x+(width/2), center.y+(height/2));
    }
    Square(Point center,double width){
        Square(center,width,width);
    }

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