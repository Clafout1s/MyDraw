#include "point.h"

bool equalF(float x,float y){
    float my_epsilon = 0.00001;
    return (fabs(x-y) <  my_epsilon || fabs(x-y) < FLT_MIN);
}

void printPoint(const Point& p){
    std::cout << p.x <<", "<<p.y<<", "<<p.z<<"\n";
}

void printPointGeogebra(const Point& p){
    std::cout << "("<<p.x<<", "<<p.y<<")";
}
