#ifndef MY_SQUARE_H
#define MY_SQUARE_H

#include <iostream>
#include <vector>
#include <tuple>
#include "point.h"
#include "color.h"

/**
 * The representation of rectangle, containing four corner points, a center point and a color.
 * The whole class considers that y is pointing up (normalized coordinates).
*/
class Rectangle{
    private:
    Color debug_color;
    Point p;
    public:
    Point top_left,top_right,down_left,down_right,center_p;
    Color main_color;
    inline static bool debug=false;
    
    Rectangle(Point tl,Point tr,Point dl,Point dr,Color color);
    Rectangle(Point center,float width,float height,Color color);
    Rectangle(Point center,float width,Color color);
    Rectangle(); // Generates a Null Rectangle
    float width();
    float height();
    bool operator== (const Rectangle& other);
    bool operator!= (const Rectangle& other);
    bool isSquare();
    bool isValid();
    bool isNull(); // Null Rectangles have a width or a height of 0. They serve as an alternative to NULL.
    bool include(const Rectangle& other);
    bool include(const Point& point);
    std::vector<float> list();
    Rectangle intersects(Rectangle other);
    Rectangle fuseRects(Rectangle& other);
};

void printRectangle(Rectangle& sq);
void printRectGeogebra(Rectangle& rect);
std::vector<Rectangle> cutFromRectangle(Rectangle& target, Rectangle& eraser);
void toogleDebugMode();
void chooseDebugMode(bool active);
bool isDebugModeActive();

#endif