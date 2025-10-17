#ifndef MY_POINT_H
#define MY_POINT_H

class Point{
    public:
        double x;
        double y;
        double z;

    Point(double xp,double yp, double zp): x(xp),y(yp),z(zp){};
    Point(double xp,double yp): x(xp),y(yp),z(0){};

    
    Point& operator+=(const Point& other){
        this->x+= other.x;
        this->y+= other.y;
        this->z+= other.z;
        return *this;
    }

};

Point operator+(const Point& first,const Point& other){
    return Point(first.x+other.x,first.y+other.y,first.z+other.z);
}

Point operator/(const Point& first,double diviser){
    if(diviser == 0){
        return first;
    }
    return Point(first.x/diviser,first.y/diviser,first.z/diviser);
}

#endif