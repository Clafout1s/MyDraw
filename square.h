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

        if((h==0 && w!=0) || (h!=0 && w==0)){
            throw std::invalid_argument("Height and width must be both at 0 (NULL Rectangle), or both positive !");
        }
        center_p = Point(top_left.x + (w/2),top_left.y + (h/2));
    };

    Rectangle(Point center,double width,double height){
        // height is in case coordinates needs deforming to make a square (such as centered coordinates on rectangle).
        if(width<0){
            throw std::invalid_argument("Width cannot be negative !");
        }
        if(height<0){
            throw std::invalid_argument("Width cannot be negative !");
        }
        if((height==0 && width!=0) || (height!=0 && width==0)){
            throw std::invalid_argument("Height and width must be both at 0 (NULL Rectangle), or both positive !");
        }
        
        center_p = center;
        
        top_left =  Point(center.x-(width/2), center.y+(height/2));
        top_right = Point(center.x+(width/2), center.y+(height/2));
        down_left = Point(center.x-(width/2), center.y-(height/2));
        down_right = Point(center.x+(width/2), center.y-(height/2));
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

    bool isNull(){
        return top_left.x == 0 && down_right.x == 0 && top_left.y == 0 && down_right.y == 0;
    }

    bool include(const Rectangle& other){
        return (other.top_left.x >= top_left.x && other.top_right.x <= top_right.x &&
            other.down_left.y >= down_left.y && other.top_left.y <= top_left.y);
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

    /**
     * Returns the Rectangle that is the intersection of this and other.
     * The Rectangle is empty if this and other are not intersecting.
     */
    Rectangle intersects(Rectangle other){
        Rectangle new_rect = Rectangle();
        if(other==*this){
            return new_rect;
        }
        if(include(other)){
            return other;
        }
        else if(other.include(*this)){
            return *this;
        }
        else{
            Rectangle small_one = top_right.x - top_left.x > other.top_right.x - other.top_left.x ? other:*this;
            Rectangle big_one = top_right.x - top_left.x > other.top_right.x - other.top_left.x ? *this:other;

            std::vector<Point> points = {small_one.top_left,small_one.top_right,small_one.down_left,small_one.down_right};
            bool inside = false;
            size_t y = 0;
            while (!inside && y< points.size())
            {
                // If one of the corners of other is fully inside this Rectangle
                if(points[y].x < big_one.top_right.x && points[y].x > big_one.top_left.x &&
                    points[y].y > big_one.down_left.y && points[y].y < big_one.top_left.y){
                        
                        if(top_left.x > other.top_left.x && top_right.x > other.top_right.x){
                            // If *this is right of other
                            printf("Left\n");
                            new_rect.top_left.x = top_left.x;
                            new_rect.down_left.x = down_left.x;
                            new_rect.top_right.x = other.top_right.x;
                            new_rect.down_right.x = other.down_right.x;
                        }
                        else if(top_left.x < other.top_left.x && top_right.x < other.top_right.x){
                            // If *this is left of other
                            printf("Right\n");
                            new_rect.top_left.x = other.top_left.x;
                            new_rect.down_left.x = other.down_left.x;
                            new_rect.top_right.x = top_right.x;
                            new_rect.down_right.x = down_right.x;
                        }
                        else{
                            // If *this and other are fully included into eachother in the x axis
                            printf("no left no right\n");
                            new_rect.top_left.x = small_one.top_left.x;
                            new_rect.down_left.x = small_one.down_left.x;
                            new_rect.top_right.x = small_one.top_right.x;
                            new_rect.down_right.x = small_one.down_right.x;
                        }
                        if(top_left.y > other.top_left.y && down_left.y > other.down_left.y){
                            // *this is up of other
                            printf("down\n");
                            new_rect.top_left.y = other.top_left.y;
                            new_rect.top_right.y = other.top_right.y;
                            new_rect.down_left.y = down_left.y;
                            new_rect.down_right.y = down_right.y;
                        }
                        else if(top_left.y < other.top_left.y && down_left.y < other.down_left.y){
                            // *this is down of other
                            printf("up\n");
                            new_rect.top_left.y = top_left.y;
                            new_rect.top_right.y = top_right.y;
                            new_rect.down_left.y = other.down_left.y;
                            new_rect.down_right.y = other.down_right.y;
                        }
                        else{
                            // If *this and other are fully included into eachother in the y axis
                            printf("no up no down\n");
                            new_rect.top_left.y = small_one.top_left.y;
                            new_rect.top_right.y = small_one.top_right.y;
                            new_rect.down_left.y = small_one.down_left.y;
                            new_rect.down_right.y = small_one.down_right.y;
                        }
                        inside = true;
                    }
                y++;
            }
        }
        return new_rect;
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