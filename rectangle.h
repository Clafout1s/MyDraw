#ifndef MY_SQUARE_H
#define MY_SQUARE_H

#include <iostream>
#include <vector>
#include <tuple>
#include "point.h"

class Rectangle{
    private:
    float colorR,colorG,colorB;
    Point p;
    public:
    Point top_left,top_right,down_left,down_right,center_p;
    inline static bool debug=false;
    
    Rectangle(Point tl,Point tr,Point dl,Point dr);
    Rectangle(Point center,float width,float height);
    Rectangle(Point center,float width);
    Rectangle();
    float width();
    float height();
    bool operator== (const Rectangle& other);
    bool operator!= (const Rectangle& other);
    bool isSquare();
    bool isValid();
    bool isNull();
    bool include(const Rectangle& other);
    bool include(const Point& point);
    std::vector<float> list(float r,float g,float b);
    Rectangle intersects(Rectangle other);
    Rectangle fuseRects(Rectangle& other);
    
};

void printRectangle(Rectangle& sq);
void printRectGeogebra(Rectangle& rect);
std::vector<Rectangle> cutFromRectangle(Rectangle& target, Rectangle& eraser);
void toogleDebugMode();
bool isDebugModeActive();

#endif