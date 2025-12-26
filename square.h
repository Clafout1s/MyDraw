#ifndef MY_SQUARE_H
#define MY_SQUARE_H

#include <iostream>
#include <vector>
#include <tuple>
#include "point.h"
#include "rectangle.h"

class Rectangle;
void printRectGeogebra(Rectangle& rect);

class Rectangle{
    public:
    Point top_left,top_right,down_left,down_right,center_p;

    Rectangle(Point tl,Point dr){
        if(dr.x < tl.x){
            throw std::invalid_argument("Down right x should be greater than top left x !");
        }
        if(dr.y < tl.y){
            throw std::invalid_argument("Down right y should be greater than top left y !");
        }
        if((dr.y == tl.y) || (dr.x==tl.x)){
            // NULL Rectangle
            top_left = Point();
            top_right = Point();
            down_left = Point();
            down_right = Point();
        }
        top_left = tl;
        down_right = dr;
        top_right = Point(down_right.x,top_left.y);
        down_left = Point(top_left.x,down_right.y);
    }

    Rectangle(Point tl,Point tr,Point dl,Point dr): top_left(tl),top_right(tr),down_left(dl),down_right(dr) {
        if(tl.x != dl.x){
            throw std::invalid_argument("Top left and down left should have the same x coordinates !");
        }
        if(tr.x != dr.x){
            throw std::invalid_argument("Top right and down right should have the same x coordinates !");
        }
        if(tl.y != tr.y){
            throw std::invalid_argument("Top left and top right should have the same y coordinates !");
        }
        if(dl.y != dr.y){
            throw std::invalid_argument("Down left and down right should have the same y coordinates !");
        }

        double w = width();
        double h = height();

        center_p = Point(top_left.x + (w/2),top_left.y + (h/2));
    };

    Rectangle(Point center,double width,double height){        
        center_p = center;
        top_left =  Point(center.x-(width/2), center.y+(height/2));
        top_right = Point(center.x+(width/2), center.y+(height/2));
        down_left = Point(center.x-(width/2), center.y-(height/2));
        down_right = Point(center.x+(width/2), center.y-(height/2));
        if(width<0){
            throw std::invalid_argument("Width should not be negative, "+std::to_string(width)+" is incorrect");
        }
        if(height<0){
            throw std::invalid_argument("Height should not be negative, "+std::to_string(height)+" is incorrect");
        }
    }

    Rectangle(Point center,double width):Rectangle(center,width,width){}

    Rectangle():Rectangle(Point(),0){}

    double width(){
        return top_right.x - top_left.x;
    }

    double height(){
        return top_left.y - down_left.y;
    }

    bool operator== (const Rectangle& other){
        return (top_left == other.top_left && top_right==other.top_right && 
        down_left==other.down_left && down_right==other.down_right);
    }

    bool isSquare(){
        return abs((top_right-top_left).x) == abs((top_left-down_left).y);
    }

    bool isValid(){
        return (width()>=0 && height()>=0 && top_left.x == down_left.x && top_right.x == down_right.x 
            && down_left.y == down_right.y && top_left.y == top_right.y);
    }

    bool isNull(){
        return width()==0 || height()==0;
    }

    bool include(const Rectangle& other){
        return (other.top_left.x >= top_left.x && other.top_right.x <= top_right.x &&
            other.down_left.y >= down_left.y && other.top_left.y <= top_left.y);
    }

    bool include(const Point& point){
        return (point.x <= top_right.x && point.x >= top_left.x && point.y <= top_left.y && point.y >= down_left.y);
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

    Rectangle intersects(Rectangle other){
        Rectangle inter = Rectangle();
        if(top_left.x >= other.top_right.x || other.top_left.x >= top_right.x ||
            top_left.y <= other.down_left.y || other.top_left.y <= down_left.y){
                // The two rectangles don't intersect with each other
                return inter;
            }
        Point *rec1_points[] = {&top_left,&top_right,&down_left,&down_right};
        Point* rec2_points[] = {&other.top_left,&other.top_right,&other.down_left,&other.down_right};
        Point* inter_points[] = {&inter.top_left,&inter.top_right,&inter.down_left,&inter.down_right};
        bool does_intersects = false;
        for (size_t i = 0; i < 4; i++)
        {
            if(other.include(*rec1_points[i])){
                *inter_points[i] = *rec1_points[i];
            }
            else if(include(*rec2_points[i])){
                *inter_points[i] = *rec2_points[i];
            }
            else{
                float xValue = (*rec1_points[i]).x <= other.top_right.x && (*rec1_points[i]).x >= other.top_left.x ? (*rec1_points[i]).x: (*rec2_points[i]).x;
                float yValue = (*rec1_points[i]).y <= other.top_right.y && (*rec1_points[i]).y >= other.down_left.y ? (*rec1_points[i]).y: (*rec2_points[i]).y;
                *inter_points[i] = Point(xValue,yValue);
            }
        }
        if(!inter.isValid()){
            printf("not valid\n");

        }
        printRectGeogebra(inter);
        return inter;
    }
};

void printRectangle(Rectangle& sq){
    printf("Rectangle: \n");
    printf("    ");
    printPoint(sq.top_left);
    printf("    ");
    printPoint(sq.top_right);
    printf("    ");
    printPoint(sq.down_left);
    printf("    ");
    printPoint(sq.down_right);
    printf("Width: %f; height: %f\n",sq.top_right.x - sq.top_left.x,sq.top_left.y - sq.down_left.y);
    printf("Center: ");
    printPoint(sq.center_p);
    printf("------\n");
}

void printRectGeogebra(Rectangle& rect){
    std::cout << "Polygone( ";
    printPointGeogebra(rect.top_left);
    std::cout << ", ";
    printPointGeogebra(rect.top_right);
    std::cout << ", ";
    printPointGeogebra(rect.down_right);
    std::cout << ", ";
    printPointGeogebra(rect.down_left);
    std::cout << ", ";
    printPointGeogebra(rect.top_left);
    std::cout << ")\n";
}

class Square: public Rectangle{
    public:
        Square(Point center,double width):Rectangle(center,width,width){};
        Square(Point tl,Point tr,Point dl,Point dr):Rectangle(tl,tr,dl,dr){};
        Square():Rectangle(Point(),0){}

        bool operator<(Square other){
            return top_right.x-top_left.x < other.top_right.x-other.top_left.x;
        }
        bool operator>(Square other){
            return top_right.x-top_left.x > other.top_right.x-other.top_left.x;
        }
};

void printSquare(Square& sq){
    printRectangle(sq);
}


/**
     * Cuts a rectangle into squares and eventually the rest into one smaller rectangle.
     * Returns a tuple with two values: 
     * the first one is a vector of squares, the second a rectangle.
     */
inline std::vector<Square> cutToSquares(const Rectangle& rect){
    std::vector<Square> squares_list = {};
    double l = abs(rect.top_right.x - rect.top_left.x);
    double h = abs(rect.top_left.y - rect.down_left.y);
    double min_side = l >= h ? h: l;
    double max_side = l >= h ? l: h;
    int n = max_side / min_side;
    for (size_t i = 0; i < n; i++)
    {
        if(h == min_side){
            Point add_left = Point(i*min_side,0);
            Point add_right = Point((i+1)*min_side,0);
            Square new_square = Square(rect.top_left+add_left,rect.top_left+add_right,rect.down_left+add_left,rect.down_left+add_right);
            squares_list.insert(squares_list.end(),new_square);
        }
        else{
            Point add_down = Point(0,i*min_side);
            Point add_top = Point(0,(i+1)*min_side);
            Square new_square = Square(rect.down_left+add_top,rect.down_right+add_top,rect.down_left+add_down,rect.down_right+add_down);
            squares_list.insert(squares_list.end(),new_square);
        }
    }
    
    if((double)n != (max_side / (float)min_side)){
        // The rectangle is not only composed of squares
        Rectangle rest = Rectangle();
        Square last_add = squares_list.back();
        if(h == min_side){
            rest.top_left = last_add.top_right;
            rest.top_right = rect.top_right;
            rest.down_left = last_add.down_right;
            rest.down_right = rect.down_right;
        }
        else{
            rest.top_left = rect.top_left;
            rest.top_right = rect.top_right;
            rest.down_left = last_add.top_left;
            rest.down_right = last_add.top_right;
        }
        std::vector<Square> recursive_squares = cutToSquares(rest);
        squares_list.insert(squares_list.end(),recursive_squares.begin(),recursive_squares.end());
    }
    return std::vector<Square>(squares_list);
}

#endif