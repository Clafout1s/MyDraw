#ifndef MY_POINT_H
#define MY_POINT_H

#include <iostream>
#include <vector>


class Point{
    public:
        double x;
        double y;
        double z;

    Point(double xp,double yp, double zp): x(xp),y(yp),z(zp){};
    Point(double xp,double yp): x(xp),y(yp),z(0){};
    Point(): x(0),y(0),z(0) {};
    
    Point& operator+=(const Point& other){
        this->x+= other.x;
        this->y+= other.y;
        this->z+= other.z;
        return *this;
    }

    bool operator==(const Point& b){
        return (x == b.x && y==b.y && z==b.z);
    }

    std::vector<double> list(){
        std::vector<double> points;

        points.insert(points.end(),x);
        points.insert(points.end(),y);
        points.insert(points.end(),z);

        return points;
    }

    

};

Point operator+(const Point& first,const Point& other){
    return Point(first.x+other.x,first.y+other.y,first.z+other.z);
}



void printPoint(const Point& p){
    std::cout << p.x <<", "<<p.y<<", "<<p.z<<"\n";
}

#endif