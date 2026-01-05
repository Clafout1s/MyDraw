#ifndef MY_POINT_H
#define MY_POINT_H

#include <iostream>
#include <vector>

const float epsilon = 0.00001;

bool equalF(float x,float y){
    return (fabs(x-y) < FLT_EPSILON * fabs(x+y) || fabs(x-y) < FLT_MIN);
}

class Point{
    public:
        float x;
        float y;
        float z;

    Point(float xp,float yp, float zp): x(xp),y(yp),z(zp){
        if(abs(x-0)<epsilon){
            x = 0;
        }
        if(abs(y-0)<epsilon){
            y = 0;
        }
        if(abs(z-0)<epsilon){
            z = 0;
        }
    };
    Point(float xp,float yp): Point(xp,yp,0){};
    Point(): x(0),y(0),z(0) {};
    
    Point& operator+=(const Point& other){
        this->x+= other.x;
        this->y+= other.y;
        this->z+= other.z;
        return *this;
    }

    Point operator+(const Point& other){
        Point p_new = Point(this->x + other.x, this->y + other.y, this->z + other.z);
        return p_new;
    }

    Point operator-(const Point& other){
        Point p_new = Point(this->x - other.x, this->y - other.y, this->z - other.z);
        return p_new;
    }

    bool operator==(const Point& b){
        return (equalF(x,b.x) && equalF(y,b.y) && equalF(z,b.z));
    }

    Point& operator=(const Point& b){
        x = b.x; y = b.y; z = b.z;
        return *this;
    }

    std::vector<float> list(std::vector<float> colors = {1.0f,0,0}){
        std::vector<float> points;

        points.insert(points.end(),x);
        points.insert(points.end(),y);
        points.insert(points.end(),colors[0]);
        points.insert(points.end(),colors[1]);
        points.insert(points.end(),colors[2]);
        //points.insert(points.end(),z);

        return points;
    }

    

};

Point operator+(const Point& first,const Point& other){
    return Point(first.x+other.x,first.y+other.y,first.z+other.z);
}



void printPoint(const Point& p){
    std::cout << p.x <<", "<<p.y<<", "<<p.z<<"\n";
}

void printPointGeogebra(const Point& p){
    std::cout << "("<<p.x<<", "<<p.y<<")";
}

#endif