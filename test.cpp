#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <time.h>
#include "point.h"
#include "square.h"

std::vector<Rectangle> eraseSquare(Square& target, Square& eraser){
    Rectangle inter = eraser.intersects(target);
    if(inter.isNull()){
        // We don't need to make new squares
        return std::vector<Rectangle>{};
    }
    if(target<eraser){
        Rectangle new1 = Rectangle();
        Rectangle new2 = Rectangle();
        if(eraser.include(target)){
            return std::vector<Rectangle>{};
        }
        if(target.top_left.x < eraser.top_left.x && target.top_right.x < eraser.top_right.x){
            // Target is left of eraser
            if(target.top_left.y < eraser.top_left.y && target.down_left.y>eraser.down_left.y){
                // Target is not in a corner               
                new1.top_left = target.top_left;
                new1.top_right = inter.top_left;
                new1.down_left = target.down_left;
                new1.down_right = inter.down_left;
            }
            else{
                // Target is in a left corner
                // Cuts the new rectangle into two: new1 is the left part, new2 the rest of it
                Point step = Point(inter.top_left.x - target.top_left.x,0);
                new1.top_left = target.top_left;
                new1.top_right = target.top_left + step;
                new1.down_left = target.down_left;
                new1.down_right = target.down_left + step;
                if(target.top_left.y > eraser.top_left.y){
                    // Top left corner 
                    // new2 is up of inter
                    new2.top_left = target.top_left + step;
                    new2.top_right = target.top_right;
                    new2.down_left = inter.top_left;
                    new2.down_right = inter.top_right;
                }
                else{
                    // Down left corner 
                    // new2 is down of inter
                    new2.top_left = inter.down_left;
                    new2.top_right = inter.down_right;
                    new2.down_left = target.down_left + step;
                    new2.down_right = target.down_right;
                }
            }
        }
        else if(target.top_left.x > eraser.top_left.x && target.top_right.x > eraser.top_right.x){
            // Target is right of eraser
            if(target.top_right.y < eraser.top_right.y && target.down_right.y>eraser.down_right.y){
                // Target is not in a corner               
                new1.top_left = inter.top_right;
                new1.top_right = target.top_right;
                new1.down_left = inter.down_right;
                new1.down_right = target.down_right;
            }
            else{
                // Target is in a right corner
                // Cuts the new rectangle into two: new1 is the left part, new2 the rest of it
                Point step = Point(inter.top_right.x - inter.top_left.x,0);
                new1.top_left = target.top_left + step;
                new1.top_right = target.top_right;
                new1.down_left = target.down_left + step;
                new1.down_right = target.down_right;
                if(target.top_right.y > eraser.top_right.y){
                    // Top right corner
                    new2.top_left = target.top_left;
                    new2.top_right = target.top_left + step;
                    new2.down_left = inter.top_left;
                    new2.down_right = inter.top_right;
                }
                else{
                    // Down right corner
                    new2.top_left = inter.down_left;
                    new2.top_right = inter.down_right;
                    new2.down_left = target.down_left;
                    new2.down_right = target.down_left + step;
                }
            }
            
        }
        else if(target.down_left.y > eraser.down_left.y && target.top_left.y > eraser.top_left.y){
            // Target is up of eraser (not in a corner)
            new1.top_left = target.top_left;
            new1.top_right = target.top_right;
            new1.down_left = inter.top_left;
            new1.down_right = inter.top_right;
        }
        else if(target.down_left.y < eraser.down_left.y && target.top_left.y < eraser.top_left.y){
            new1.top_left = inter.down_left;
            new1.top_right = inter.down_right;
            new1.down_left = target.down_left;
            new1.down_right = target.down_right;
        }
        else{
            throw std::logic_error("WAT");
        }
        if(new2.isNull()){
            return std::vector<Rectangle> {new1};
        }
        else{
            return std::vector<Rectangle> {new1,new2};
        }
    }
    else{
        // target >= eraser
        std::vector<Rectangle> new_rects = {};
        if(target.top_left.x < eraser.top_left.x && target.top_right.x < eraser.top_right.x){
            // Target is left of eraser
            Point step = Point(inter.width(),0); 
            Rectangle recBigRest = Rectangle(target.top_left,target.top_right-step,target.down_left,target.down_right-step);
            new_rects.insert(new_rects.end(),recBigRest);
            if(target.top_left.y > eraser.top_left.y && target.down_left.y<eraser.down_left.y){
                // Target is not in a corner     
                Rectangle recSmall1 = Rectangle(target.top_right-step,target.top_right,inter.top_left,inter.top_left+step);
                Rectangle recSmall2 = Rectangle(inter.down_left,inter.down_left+step,target.down_right-step,target.down_right);
                new_rects.insert(new_rects.end(),recSmall1);
                new_rects.insert(new_rects.end(),recSmall2);
            }
            else{
                // Target is in a left corner
                if(target.top_left.y > eraser.top_left.y){
                    // Top left corner 
                    Rectangle recSmall = Rectangle(target.top_right-step,target.top_right,inter.top_left,inter.top_left+step);
                    new_rects.insert(new_rects.end(),recSmall);
                }
                else{
                    // Down left corner 
                    Rectangle recSmall = Rectangle(inter.down_left,inter.down_left+step,target.down_right-step,target.down_right);
                    new_rects.insert(new_rects.end(),recSmall);
                }
            }
        }
        else if(target.top_left.x > eraser.top_left.x && target.top_right.x > eraser.top_right.x){
            // Target is right of eraser
            Point step = Point(inter.width(),0);
            Rectangle recBigRest = Rectangle(target.top_left+step,target.top_right,target.down_left+step,target.down_right);
            new_rects.insert(new_rects.end(),recBigRest);
            if(target.top_right.y > eraser.top_right.y && target.down_right.y<eraser.down_right.y){
                // Target is not in a corner       
                Rectangle recSmall1 = Rectangle(target.top_left,target.top_left+step,inter.top_right-step,inter.top_right);
                Rectangle recSmall2 = Rectangle(inter.down_right-step,inter.down_right,target.down_left,target.down_left+step);
                new_rects.insert(new_rects.end(),recSmall1);
                new_rects.insert(new_rects.end(),recSmall2);
            }
            else{
                // Target is in a right corner
                // Cuts the new rectangle into two: new1 is the left part, new2 the rest of it
                if(target.top_right.y > eraser.top_right.y){
                    // Top right corner
                    Rectangle recSmall = Rectangle(target.top_left,target.top_left+step,inter.top_right-step,inter.top_right);
                    new_rects.insert(new_rects.end(),recSmall);
                }
                else{
                    // Down right corner
                    Rectangle recSmall = Rectangle(inter.down_right-step,inter.down_right,target.down_left,target.down_left+step);
                    new_rects.insert(new_rects.end(),recSmall);
                }
            }
        }
        else if(target.down_left.y > eraser.down_left.y && target.top_left.y > eraser.top_left.y){
            // Target is up of eraser (not in a corner)
            Point step = Point(0,inter.height());
            Rectangle recBig = Rectangle(target.top_left,target.top_right,target.down_left+step,target.down_right+step);
            Rectangle recSmall1 = Rectangle(target.down_left+step,inter.top_left,target.down_left,inter.down_left);
            Rectangle recSmall2 = Rectangle(inter.top_right,target.down_right+step,inter.down_right,target.down_right);
            new_rects.insert(new_rects.end(),recBig);
            new_rects.insert(new_rects.end(),recSmall1);
            new_rects.insert(new_rects.end(),recSmall2);
        }
        else if(target.down_left.y < eraser.down_left.y && target.top_left.y < eraser.top_left.y){
            // Target is down of eraser (not in a corner)
            Point step = Point(0,inter.height());
            Rectangle recBig = Rectangle(target.top_left-step,target.top_right-step,target.down_left,target.down_right);
            Rectangle recSmall1 = Rectangle(target.top_left,inter.top_left,target.top_left-step,inter.down_left);
            printf("in %f %f\n",(target.down_right-step).y,inter.down_right.y);
            Rectangle recSmall2 = Rectangle(inter.top_right,target.top_right,inter.down_right,target.top_right-step);
            new_rects.insert(new_rects.end(),recBig);
            new_rects.insert(new_rects.end(),recSmall1);
            new_rects.insert(new_rects.end(),recSmall2);
        }
        else{
            // Eraser is fully included in target
            Point stepLeft = Point(inter.top_left.x-target.top_left.x,0);
            Point stepInterW = Point(inter.width(),0);
            Rectangle recBigLeft = Rectangle(target.top_left,target.top_left+stepLeft,target.down_left,target.down_left+stepLeft);
            Rectangle recBigRight = Rectangle(target.top_left+stepInterW+stepLeft,target.top_right,target.down_left+stepLeft+stepInterW,target.down_right);
            Rectangle smallRecUp = Rectangle(target.top_left+stepLeft,target.top_left+stepInterW+stepLeft,inter.top_left,inter.top_right);
            Rectangle smallRecDown = Rectangle(inter.down_left,inter.down_right,target.down_left+stepLeft,target.down_left+stepLeft+stepInterW);
            new_rects.insert(new_rects.end(),recBigLeft);
            new_rects.insert(new_rects.end(),recBigRight);
            new_rects.insert(new_rects.end(),smallRecUp);
            new_rects.insert(new_rects.end(),smallRecDown);
        
        }
        return new_rects;
    }
    return std::vector<Rectangle> {};
}


int main()
{
    srand(time(0));

    Square eraser = Square(Point(rand()%10,rand()%10,0),(rand()%20)+1);
    Square big = Square(Point(0,0),(rand()%20)+1);
    printRectGeogebra(eraser);
    printRectGeogebra(big);
    std::vector<Rectangle> result = eraseSquare(big,eraser);
    printf("Erase\n");
    for (size_t i = 0; i < result.size(); i++)
    {
        printRectGeogebra(result[i]);
    }

    return 0;
}
