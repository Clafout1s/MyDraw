#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <time.h>
#include "point.h"
#include "square.h"

std::vector<Rectangle> eraseRectangle(Rectangle& target, Rectangle& eraser){
    Rectangle inter = eraser.intersects(target);
    std::vector<Rectangle> new_rects = {};
    if(inter.isNull()){
        // We don't need to make new squares
        return std::vector<Rectangle>{};
    }
    if(inter==target){
        printf("equals\n");
        return std::vector<Rectangle>{}; 
    }

    if(eraser.top_left.x <= target.top_left.x && eraser.top_right.x > target.top_left.x){
        // Eraser is left of target
        Point stepRight = Point(inter.width(),0);
        Point heightUp = Point(0,target.top_left.y-inter.top_left.y);
        Point heightDown = Point(0,eraser.down_left.y - target.down_left.y);
        if(target.top_right.x > eraser.top_right.x){
            // Rectangle right of eraser
            Rectangle right = Rectangle(target.top_left+stepRight,target.top_right,target.down_left+stepRight,target.down_right);
            new_rects.insert(new_rects.end(),right);
        }
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up of eraser
            Rectangle top = Rectangle(target.top_left,target.top_left+stepRight,target.top_left-heightUp,target.top_left+stepRight-heightUp);
            new_rects.insert(new_rects.end(),top);
        }
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down of eraser
            Rectangle down = Rectangle(target.down_left+heightDown,target.down_left+heightDown+stepRight,target.down_left,target.down_left+stepRight);
            new_rects.insert(new_rects.end(),down);
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
            new_rects.insert(new_rects.end(),left);
        }
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up of eraser
            Rectangle top = Rectangle(target.top_right-stepLeft,target.top_right,target.top_right-stepLeft-heightUp,target.top_right-heightUp);
            new_rects.insert(new_rects.end(),top);
        }
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down of eraser
            Rectangle down = Rectangle(target.down_right+heightDown-stepLeft,target.down_right+heightDown,target.down_right-stepLeft,target.down_right);
            new_rects.insert(new_rects.end(),down);
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
            new_rects.insert(new_rects.end(),top);
        }
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left
            Rectangle left = Rectangle(target.down_left+stepTop,target.down_left+stepTop+widthLeft,target.down_left,target.down_left+widthLeft);
            new_rects.insert(new_rects.end(),left);
        }
        if(target.top_right.x >eraser.top_right.x){
            // Rectangle right
            Rectangle right = Rectangle(target.down_right-widthRight+stepTop,target.down_right+stepTop,target.down_right-widthRight,target.down_right);
            new_rects.insert(new_rects.end(),right);
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
            new_rects.insert(new_rects.end(),down);
        }
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left
            Rectangle left = Rectangle(target.top_left,target.top_left+widthLeft,target.top_left-stepDown,target.top_left-stepDown+widthLeft);
            new_rects.insert(new_rects.end(),left);
        }
        if(target.top_right.x >eraser.top_right.x){
            // Rectangle right
            Rectangle right = Rectangle(target.top_right-widthRight,target.top_right,target.top_right-stepDown-widthRight,target.top_right-stepDown);
            new_rects.insert(new_rects.end(),right);
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
        new_rects.insert(new_rects.end(),left);
        new_rects.insert(new_rects.end(),right);
        new_rects.insert(new_rects.end(),top);
        new_rects.insert(new_rects.end(),down);
    }
    else{
        printf("ERROR in eraseRectangle with these eraser and target: \n");
        printRectangle(eraser);
        printRectangle(target);
        throw std::logic_error("\n");
    }
    return new_rects;
}


int main()
{
    srand(time(0));
    Rectangle eraser = Rectangle(Point(rand()%5 -5,rand()%5 -5),rand()%10 +1,rand()%10 +1);
    Rectangle big = Rectangle(Point(rand()%5 -5,rand()%5 -5),rand()%10 +1,rand()%10 +1);
    printRectGeogebra(eraser);
    printRectGeogebra(big);
    std::vector<Rectangle> result = eraseRectangle(big,eraser);
    printf("Erase\n");
    for (size_t i = 0; i < result.size(); i++)
    {
        printRectGeogebra(result[i]);
    }
    return 0;
}
