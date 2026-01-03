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

        float w = width();
        float h = height();

        center_p = Point(top_left.x + (w/2),top_left.y + (h/2));
    };

    Rectangle(Point center,float width,float height){        
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

    Rectangle(Point center,float width):Rectangle(center,width,width){}

    Rectangle():Rectangle(Point(),0){}

    float width(){
        float wtemp = top_right.x - top_left.x;
        if(abs(wtemp-0)<epsilon){
            return 0;
        }
        else{
            return wtemp;
        }
    }

    float height(){
        float htemp = top_left.y - down_left.y;
        if(abs(htemp-0)<epsilon){
            return 0;
        }
        else{
            return htemp;
        }
    }

    bool operator== (const Rectangle& other){
        return (top_left == other.top_left && top_right==other.top_right && 
        down_left==other.down_left && down_right==other.down_right);
    }

    bool isSquare(){
        return abs((top_right-top_left).x) == abs((top_left-down_left).y);
    }

    bool isValid(){
        return (width()>=0 && height()>=0 && (float)top_left.x == (float)down_left.x && (float)top_right.x == (float)down_right.x 
            && (float)down_left.y == (float)down_right.y && (float)top_left.y == (float)top_right.y);
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

    

    std::vector<float> list(){
        std::vector<float> points;

        std::vector<float> ptl = top_left.list();
        points.insert(points.end(),ptl.begin(),ptl.end());

        std::vector<float> ptr = top_right.list();
        points.insert(points.end(),ptr.begin(),ptr.end());

        std::vector<float> pdl = down_left.list();
        points.insert(points.end(),pdl.begin(),pdl.end());

        std::vector<float> pdr = down_right.list();
        points.insert(points.end(),pdr.begin(),pdr.end());

        return points;
    }

    Rectangle intersects(Rectangle other){
        Rectangle inter = Rectangle();
        if((float)top_left.x >= (float)other.top_right.x || (float)other.top_left.x >= (float)top_right.x ||
            (float)top_left.y <= (float)other.down_left.y || (float)other.top_left.y <= (float)down_left.y){
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
        if(!inter.isValid() || inter.isNull()){
            /*
            std::cout << std::fixed << std::setprecision(10);
            printf("\nInter not valid. Printing this Rectangle, other and inter: \n");
            printRectGeogebra(*this);
            printRectGeogebra(other);
            printRectGeogebra(inter);
            //throw std::logic_error("\n");
            */
            return Rectangle();
        }
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
        Square(Point center,float width):Rectangle(center,width,width){};
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
    float l = abs(rect.top_right.x - rect.top_left.x);
    float h = abs(rect.top_left.y - rect.down_left.y);
    float min_side = l >= h ? h: l;
    float max_side = l >= h ? l: h;
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
    
    if((float)n != (max_side / (float)min_side)){
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

/**
 * Finds the intersection between target and eraser, and returns a list of rectangles that are the rest of target,
 *  without the intersection.
 * If no deletion is needed, the vector is empty.
 * If there is deletion but no replacement, the vector contains a single NULL Rectangle, at index 0
 */
std::vector<Rectangle> cutFromRectangle(Rectangle& target, Rectangle& eraser){
    Rectangle inter = eraser.intersects(target);
    std::vector<Rectangle> new_rects = {};
    if(inter.isNull()){
        // No deletion needed
        return std::vector<Rectangle>{};
    }
    if(inter==target){
        // Deletion, but no replacement
        return std::vector<Rectangle>{Rectangle()}; 
    }

    if(eraser.top_left.x <= target.top_left.x && eraser.top_right.x > target.top_left.x){
        // Eraser is left of target
        Point stepRight = Point(inter.width(),0);
        Point heightUp = Point(0,target.top_left.y-inter.top_left.y);
        Point heightDown = Point(0,eraser.down_left.y - target.down_left.y);
        if(target.top_right.x > eraser.top_right.x){
            // Rectangle right of eraser
            Rectangle right = Rectangle(target.top_left+stepRight,target.top_right,target.down_left+stepRight,target.down_right);
            if(!right.isNull()){
                new_rects.insert(new_rects.end(),right);
            }
        }
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up of eraser
            Rectangle top = Rectangle(target.top_left,target.top_left+stepRight,target.top_left-heightUp,target.top_left+stepRight-heightUp);
            if(!top.isNull()){
                new_rects.insert(new_rects.end(),top);
            }
        }
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down of eraser
            Rectangle down = Rectangle(target.down_left+heightDown,target.down_left+heightDown+stepRight,target.down_left,target.down_left+stepRight);
            if(!down.isNull()){
                new_rects.insert(new_rects.end(),down);
            }
        }
    }
    else if(eraser.top_right.x >= target.top_right.x && eraser.top_left.x < target.top_right.x){
        // Eraser is right of target
        Point stepLeft = Point(inter.width(),0);
        Point heightUp = Point(0,target.top_left.y-inter.top_left.y);
        Point heightDown = Point(0,eraser.down_left.y - target.down_left.y);
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left of eraser
            Rectangle left = Rectangle(target.top_left,target.top_right-stepLeft,target.down_left,target.down_right-stepLeft);
            if(!left.isNull()){
                new_rects.insert(new_rects.end(),left);
            }
        }
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up of eraser
            Rectangle top = Rectangle(target.top_right-stepLeft,target.top_right,target.top_right-stepLeft-heightUp,target.top_right-heightUp);
            if(!top.isNull()){
                new_rects.insert(new_rects.end(),top);
            }
        }
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down of eraser
            Rectangle down = Rectangle(target.down_right+heightDown-stepLeft,target.down_right+heightDown,target.down_right-stepLeft,target.down_right);
            if(!down.isNull()){
                new_rects.insert(new_rects.end(),down);
            }
        }
    }
    else if(eraser.down_left.y<=target.down_left.y && eraser.top_left.y > target.down_left.y){
        // Eraser is down of target
        Point stepTop = Point(0,inter.height());
        Point widthLeft = Point(eraser.top_left.x - target.top_left.x,0);
        Point widthRight = Point(target.top_right.x-eraser.top_right.x,0);
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up
            Rectangle top = Rectangle(target.top_left,target.top_right,target.down_left+stepTop,target.down_right+stepTop);
            if(!top.isNull()){
                new_rects.insert(new_rects.end(),top);
            }
        }
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left
            Rectangle left = Rectangle(target.down_left+stepTop,target.down_left+stepTop+widthLeft,target.down_left,target.down_left+widthLeft);
            if(!left.isNull()){
                new_rects.insert(new_rects.end(),left);
            }
        }
        if(target.top_right.x >eraser.top_right.x){
            // Rectangle right
            Rectangle right = Rectangle(target.down_right-widthRight+stepTop,target.down_right+stepTop,target.down_right-widthRight,target.down_right);
            if(!right.isNull()){
                new_rects.insert(new_rects.end(),right);
            }
        }
    }
    else if(eraser.top_left.y>=target.top_left.y && eraser.down_left.y < target.top_left.y){
        // Eraser is up of target
        Point stepDown = Point(0,inter.height());
        Point widthLeft = Point(eraser.top_left.x - target.top_left.x,0);
        Point widthRight = Point(target.top_right.x-eraser.top_right.x,0);
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle down
            Rectangle down = Rectangle(target.top_left-stepDown,target.top_right-stepDown,target.down_left,target.down_right);
            if(!down.isNull()){
                new_rects.insert(new_rects.end(),down);
            }
        }
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left
            Rectangle left = Rectangle(target.top_left,target.top_left+widthLeft,target.top_left-stepDown,target.top_left-stepDown+widthLeft);
            if(!left.isNull()){
                new_rects.insert(new_rects.end(),left);
            }
        }
        if(target.top_right.x >eraser.top_right.x){
            // Rectangle right
            Rectangle right = Rectangle(target.top_right-widthRight,target.top_right,target.top_right-stepDown-widthRight,target.top_right-stepDown);
            if(!right.isNull()){
                new_rects.insert(new_rects.end(),right);
            }
        }
    }
    else if(eraser==inter){
        // Eraser is fully included in target
        Point heightUp = Point(0,target.top_left.y-eraser.top_left.y);
        Point heightDown = Point(0,eraser.down_left.y - target.down_left.y);
        Point widthLeft = Point(eraser.top_left.x - target.top_left.x,0);
        Point widthRight = Point(target.top_right.x-eraser.top_right.x,0);

        Rectangle left = Rectangle(target.top_left,target.top_left+widthLeft,target.down_left,target.down_left+widthLeft);
        Rectangle right = Rectangle(target.top_right-widthRight,target.top_right,target.down_right-widthRight,target.down_right);
        Rectangle top = Rectangle(target.top_left+widthLeft,target.top_right-widthRight,target.top_left+widthLeft-heightUp,target.top_right-widthRight-heightUp);
        Rectangle down = Rectangle(target.down_left+widthLeft+heightDown,target.down_right+heightDown-widthRight,target.down_left+widthLeft,target.down_right-widthRight);
        if(!left.isNull()){
            new_rects.insert(new_rects.end(),left);
        }
        if(!right.isNull()){
            new_rects.insert(new_rects.end(),right);
        }
        if(!top.isNull()){
            new_rects.insert(new_rects.end(),top);
        }
        if(!down.isNull()){
            new_rects.insert(new_rects.end(),down);
        }
    }
    else{
        printf("\nERROR in cutFromRectangle: case is not handled !\n Arguments eraser and target: \n");
        printRectangle(eraser);
        printRectangle(target);
        throw std::logic_error("\n");
    }
    return new_rects;
}

#endif