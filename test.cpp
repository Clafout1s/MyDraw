#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <iomanip>
#include <float.h>
#include "vertexData.h"

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

    Rectangle square = Rectangle(point,width,height,{0,0,0});
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


int main()
{
    srand(time(0));
    int nb = 5;
    std::vector<Rectangle> rects = {};
    std::vector<unsigned int> positions = {};
    for (size_t i = 0; i < nb; i++)
    {
        rects.insert(rects.end(),Rectangle(Point(),rand()%20,{0,0,0}));
        positions.insert(positions.end(),rand()%10);
    }
    vertexData data = vertexData(rects);

    std::cout <<"\n";
    for (size_t i = 0; i < rects.size(); i++)
    {
        printRectGeogebra(rects[i]);
        std::cout << positions[i]<<"\n";
    }
    std::cout << "\n";
    std::tuple<std::vector<Rectangle>,std::vector<unsigned int>> sorted = data.sortRectArrayByPosition(rects,positions);
    std::vector<Rectangle> sorted_rects = std::get<0>(sorted);
    std::vector<unsigned int> sorted_pos = std::get<1>(sorted);
    for (size_t i = 0; i < sorted_rects.size(); i++)
    {
        printRectGeogebra(sorted_rects[i]);
        std::cout << sorted_pos[i]<<"\n";
    }
    
     
    return 0;
}
