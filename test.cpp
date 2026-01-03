#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <iomanip>
#include "point.h"
#include "square.h"

int sign(float num){
    if(num<0){
        return -1;
    }
    else if(num>0){
        return 1;
    }
    else{
        return 0;
    }
}

Rectangle calculateSquare(const Point& point,int width_pixel,float width_screen,float ratio){
    float width = 2* (float)width_pixel/width_screen; // normalize width
    float height = width*ratio;

    Rectangle square = Rectangle(point,width,height);
    return square;
}

Point normalizePosition(Point point_global, float width, float height){
    Point normalized = Point(point_global.x,point_global.y);
    normalized.x = 2*normalized.x/width -1;
    normalized.y = -(2*normalized.y/height -1);
    return normalized;
}

Point nearestTile(Point point,unsigned int size){
    // Takes arguments in pixel coordinates
    Point center = Point(0,0);
    if(std::fmod(point.x,size) == 0){
        center.x = abs(point.x - size/2.f);
    }
    if(std::fmod(point.y,size) == 0){
        center.y = abs(point.y - size/2.f);
    }
    if(center.x == 0){
        unsigned int line_square_x = abs(round(point.x/size)*size);
        center.x = size/2.f *sign(point.x-line_square_x) + (line_square_x);
    }
    if(center.y == 0){
        unsigned int line_square_y = abs(round(point.y/size)*size);
        center.y = size/2.f *sign(point.y-line_square_y) + (line_square_y);
    }

    return center;
}

/*
(404.332, 352.836)
Polygone( (-2.42144e-08, 0.15), (0.05, 0.15), (0.05, 0.1), (-2.42144e-08, 0.1), (-2.42144e-08, 0.15))
*/
int main()
{
    srand(time(0));

    Point pixel = Point(404.332, 352.836);
    pixel = nearestTile(pixel,20);
    printPoint(pixel);
    std::cout << "\n";
    Point norm = normalizePosition(pixel,800,800);
    printPoint(norm);
    std::cout << "\n";
    Rectangle recNorm = calculateSquare(norm,20,800,1);
    //std::cout << std::fixed << std::setprecision(5);
    printRectangle(recNorm);
    float epsilon = 0.00001;
    if(abs(recNorm.top_left.x - 0) < epsilon){
        printf("Zero !\n");
    }
    return 0;
}
