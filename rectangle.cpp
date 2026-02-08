#include "rectangle.h"

    /**
     * Generates a Rectangle based on its 4 corner points.
     */
    Rectangle::Rectangle(Point tl,Point tr,Point dl,Point dr,Color color):top_left(tl),top_right(tr),down_left(dl),down_right(dr),main_color(color){
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
        debug_color.red = ((double)rand())/RAND_MAX;
        debug_color.green = ((double)rand())/RAND_MAX;
        debug_color.blue = ((double)rand())/RAND_MAX;
    }

    /**
     * Generates a Rectangle based on its center point, a width and a height.
    */
    Rectangle::Rectangle(Point center,float width,float height,Color color):main_color(color){        
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
        debug_color.red = ((double)rand())/RAND_MAX;
        debug_color.green = ((double)rand())/RAND_MAX;
        debug_color.blue = ((double)rand())/RAND_MAX;
    }

    /**
     * Generates a square Rectangle, with width as height.
     */
    Rectangle::Rectangle(Point center,float width,Color color):Rectangle(center,width,width,color){}

    /**
     * Generates a Null Rectangle, an alternative to NULL.
     */
    Rectangle::Rectangle():Rectangle(Point(),0,{0,0,0}){}

    /**
     * Returns the width of the Rectangle.
     */
    float Rectangle::width(){
        float wtemp = top_right.x - top_left.x;
        if(wtemp<epsilon){
            return 0;
        }
        else{
            return wtemp;
        }
    }

    /**
     * Returns the height of the Rectangle.
     */
    float Rectangle::height(){
        float htemp = top_left.y - down_left.y;
        if(htemp<epsilon){
            return 0;
        }
        else{
            return htemp;
        }
    }

    bool Rectangle::operator== (const Rectangle& other){
        return (top_left == other.top_left && top_right==other.top_right && 
        down_left==other.down_left && down_right==other.down_right);
    }

    bool Rectangle::operator!= (const Rectangle& other){
        return !(top_left == other.top_left && top_right==other.top_right && 
        down_left==other.down_left && down_right==other.down_right);
    }

    bool Rectangle::isSquare(){
        return width() == height();
    }

    /**
     * Returns true if the Rectangle has valid corner points (a Null Rectangle can be valid).
     */
    bool Rectangle::isValid(){
        return (width()>=0 && height()>=0 && equalF((float)top_left.x,(float)down_left.x) && equalF((float)top_right.x,(float)down_right.x) 
            && equalF((float)down_left.y, (float)down_right.y) && equalF((float)top_left.y, (float)top_right.y));
    }

    /**
     * Returns true if the Rectangle has either a width or a height of 0 (it is considered a Null Rectangle).
     */
    bool Rectangle::isNull(){
        return width()==0 || height()==0;
    }

    /**
     * Returns true if the other Rectangle is entirely within this Rectangle. 
     */
    bool Rectangle::include(const Rectangle& other){
        return (other.top_left.x >= top_left.x && other.top_right.x <= top_right.x &&
            other.down_left.y >= down_left.y && other.top_left.y <= top_left.y);
    }

    /**
     * Returns true if the Point is entirely within this Rectangle.
     */
    bool Rectangle::include(const Point& point){
        return (point.x <= top_right.x && point.x >= top_left.x && point.y <= top_left.y && point.y >= down_left.y);
    }

    /**
     * Lists the data that will be used by OpenGL to draw the Rectangle (vertices + colors).
     */
    std::vector<float> Rectangle::list(){
        std::vector<float> points;
        Color colors;
        if(debug){
            colors = debug_color;
        }
        else{
            colors = main_color;
        }

        std::vector<float> ptl = top_left.list(colors);
        points.insert(points.end(),ptl.begin(),ptl.end());

        std::vector<float> ptr = top_right.list(colors);
        points.insert(points.end(),ptr.begin(),ptr.end());

        std::vector<float> pdl = down_left.list(colors);
        points.insert(points.end(),pdl.begin(),pdl.end());

        std::vector<float> pdr = down_right.list(colors);
        points.insert(points.end(),pdr.begin(),pdr.end());

        return points;
    }

    /**
     * Returns the intersection between this Rectangle, and the other Rectangle, as a Rectangle.
     * Returns a Null Rectangle if there is no intersection between the two.
     */
    Rectangle Rectangle::intersects(Rectangle other){
        Rectangle inter = Rectangle();
        if((float)top_left.x >= (float)other.top_right.x || (float)other.top_left.x >= (float)top_right.x ||
            (float)top_left.y <= (float)other.down_left.y || (float)other.top_left.y <= (float)down_left.y){
                // The two rectangles don't intersect with each other
                return inter;
        }
        Point *rec1_points[] = {&top_left,&top_right,&down_left,&down_right};
        Point* rec2_points[] = {&other.top_left,&other.top_right,&other.down_left,&other.down_right};
        Point* inter_points[] = {&inter.top_left,&inter.top_right,&inter.down_left,&inter.down_right};
        
        // Checks if one of the points of a Rectangle is included in another, and calculates the points of the intersection.
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
            return Rectangle();
        }
        inter.main_color = this->main_color;
        return inter;
    }

    /**
     * Fuses two rectangles together and returns the fusion.
     * A fusion takes two Rectangles who share a dimension (same x values for example), and creates a Rectangle whith
     * the same dimension, but takes for its second dimension all the space between the two on that dimension.
     * Even if the two rectangles aren't intersecting or in contact, a fusion can be done.
     * It returns a Null Rectangle when the fusion is impossible.
     * Visual example: |_| |_|  -> |___|
     */
    Rectangle Rectangle::fuseRects(Rectangle& other){
        Rectangle new_rect = Rectangle();
        if(!equalF(this->width(),other.width()) && !equalF(this->height(),other.height())){
            // Can't fuse two rectangles which don't share a dimension: returns NULL equivalent
            return new_rect;
        }
        if((*this)==other){
            return *this;
        }

        if(this->width() == other.width() && this->top_left.x == other.top_left.x){
            // Vertical axis: this and other share the same width and positions in x

            new_rect.top_left = Point(this->top_left.x,(this->top_left.y > other.top_left.y)?this->top_left.y:other.top_left.y);
            new_rect.top_right = Point(this->top_right.x,(this->top_right.y > other.top_right.y)?this->top_right.y:other.top_right.y);
            new_rect.down_left = Point(this->down_left.x,(this->down_left.y < other.down_left.y)?this->down_left.y:other.down_left.y);
            new_rect.down_right = Point(this->down_right.x,(this->down_right.y < other.down_right.y)?this->down_right.y:other.down_right.y);
        }
        
        else if(this->height() == other.height() && this->top_left.y == other.top_left.y){
            // Horizontal axis: this and other share the same height and positions in y

            new_rect.top_left = Point((this->top_left.x < other.top_left.x)?this->top_left.x:other.top_left.x,this->top_left.y);
            new_rect.top_right = Point((this->top_right.x > other.top_right.x)?this->top_right.x:other.top_right.x,this->top_right.y);
            new_rect.down_left = Point((this->down_left.x<other.down_left.x)?this->down_left.x:other.down_left.x,this->down_left.y);
            new_rect.down_right = Point((this->down_right.x>other.down_right.x)?this->down_right.x:other.down_right.x,this->down_right.y);
        }
        new_rect.main_color = this->main_color;
        return new_rect;
    }

/**
 * Prints the Rectangle data to the console for debugging.
 */
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

/**
 * Prints the Rectangle to the console, in the format used by the website GeoGebra.
 * This means you can copy the console text, and enter it on the website to see the Rectangle in context.
 */
void printRectGeogebra(Rectangle& rect){
    std::cout << "Polygon( ";
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

/**
 * Toogles the Debug mode, for all Rectangles.
 */
void toogleDebugMode(){
    Rectangle::debug = !Rectangle::debug;
}

/**
 * Manually choose the state of the Debug Mode.
 */
void chooseDebugMode(bool active){
    Rectangle::debug = active;
}

/**
 * Returns true if the Debug mode is on.
 */
bool isDebugModeActive(){
    return Rectangle::debug;
}

/**
 * Finds the intersection between target and eraser, and returns a list of rectangles that are the rest of target,
 * without the intersection.
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
    if((eraser.top_left.x < target.top_left.x || equalF(eraser.top_left.x,target.top_left.x)) && eraser.top_right.x > target.top_left.x){
        // Eraser is left of target
        Point stepRight = Point(inter.width(),0);
        Point heightUp = Point(0,target.top_left.y-inter.top_left.y);
        Point heightDown = Point(0,eraser.down_left.y - target.down_left.y);
        if(target.top_right.x > eraser.top_right.x){
            // Rectangle right of eraser
            Rectangle right = Rectangle(target.top_left+stepRight,target.top_right,target.down_left+stepRight,target.down_right,target.main_color);
            if(!right.isNull()){
                new_rects.insert(new_rects.end(),right);
            }
        }
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up of eraser
            Rectangle top = Rectangle(target.top_left,target.top_left+stepRight,target.top_left-heightUp,target.top_left+stepRight-heightUp,target.main_color);
            if(!top.isNull()){
                new_rects.insert(new_rects.end(),top);
            }
        }
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down of eraser
            Rectangle down = Rectangle(target.down_left+heightDown,target.down_left+heightDown+stepRight,target.down_left,target.down_left+stepRight,target.main_color);
            if(!down.isNull()){
                new_rects.insert(new_rects.end(),down);
            }
        }
    }
    else if((eraser.top_right.x > target.top_right.x || equalF(eraser.top_right.x,target.top_right.x)) && eraser.top_left.x < target.top_right.x){
        // Eraser is right of target
        Point stepLeft = Point(inter.width(),0);
        Point heightUp = Point(0,target.top_left.y-inter.top_left.y);
        Point heightDown = Point(0,eraser.down_left.y - target.down_left.y);
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left of eraser
            Rectangle left = Rectangle(target.top_left,target.top_right-stepLeft,target.down_left,target.down_right-stepLeft,target.main_color);
            if(!left.isNull()){
                new_rects.insert(new_rects.end(),left);
            }
        }
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up of eraser
            Rectangle top = Rectangle(target.top_right-stepLeft,target.top_right,target.top_right-stepLeft-heightUp,target.top_right-heightUp,target.main_color);
            if(!top.isNull()){
                new_rects.insert(new_rects.end(),top);
            }
        }
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down of eraser
            Rectangle down = Rectangle(target.down_right+heightDown-stepLeft,target.down_right+heightDown,target.down_right-stepLeft,target.down_right,target.main_color);
            if(!down.isNull()){
                new_rects.insert(new_rects.end(),down);
            }
        }
    }
    else if((eraser.down_left.y<target.down_left.y || equalF(eraser.down_left.y,target.down_left.y)) && eraser.top_left.y > target.down_left.y){
        // Eraser is down of target
        Point stepTop = Point(0,inter.height());
        Point widthLeft = Point(eraser.top_left.x - target.top_left.x,0);
        Point widthRight = Point(target.top_right.x-eraser.top_right.x,0);
        if(target.top_left.y > eraser.top_left.y){
            // Rectangle up
            Rectangle top = Rectangle(target.top_left,target.top_right,target.down_left+stepTop,target.down_right+stepTop,target.main_color);
            if(!top.isNull()){
                new_rects.insert(new_rects.end(),top);
            }
        }
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left
            Rectangle left = Rectangle(target.down_left+stepTop,target.down_left+stepTop+widthLeft,target.down_left,target.down_left+widthLeft,target.main_color);
            if(!left.isNull()){
                new_rects.insert(new_rects.end(),left);
            }
        }
        if(target.top_right.x >eraser.top_right.x){
            // Rectangle right
            Rectangle right = Rectangle(target.down_right-widthRight+stepTop,target.down_right+stepTop,target.down_right-widthRight,target.down_right,target.main_color);
            if(!right.isNull()){
                new_rects.insert(new_rects.end(),right);
            }
        }
    }
    else if((eraser.top_left.y>target.top_left.y || equalF(eraser.top_left.y,target.top_left.y)) && eraser.down_left.y < target.top_left.y){
        // Eraser is up of target
        Point stepDown = Point(0,inter.height());
        Point widthLeft = Point(eraser.top_left.x - target.top_left.x,0);
        Point widthRight = Point(target.top_right.x-eraser.top_right.x,0);
        if(target.down_left.y < eraser.down_left.y){
            // Rectangle down
            Rectangle down = Rectangle(target.top_left-stepDown,target.top_right-stepDown,target.down_left,target.down_right,target.main_color);
            if(!down.isNull()){
                new_rects.insert(new_rects.end(),down);
            }
        }
        if(target.top_left.x < eraser.top_left.x){
            // Rectangle left
            Rectangle left = Rectangle(target.top_left,target.top_left+widthLeft,target.top_left-stepDown,target.top_left-stepDown+widthLeft,target.main_color);
            if(!left.isNull()){
                new_rects.insert(new_rects.end(),left);
            }
        }
        if(target.top_right.x >eraser.top_right.x){
            // Rectangle right
            Rectangle right = Rectangle(target.top_right-widthRight,target.top_right,target.top_right-stepDown-widthRight,target.top_right-stepDown,target.main_color);
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

        Rectangle left = Rectangle(target.top_left,target.top_left+widthLeft,target.down_left,target.down_left+widthLeft,target.main_color);
        Rectangle right = Rectangle(target.top_right-widthRight,target.top_right,target.down_right-widthRight,target.down_right,target.main_color);
        Rectangle top = Rectangle(target.top_left+widthLeft,target.top_right-widthRight,target.top_left+widthLeft-heightUp,target.top_right-widthRight-heightUp,target.main_color);
        Rectangle down = Rectangle(target.down_left+widthLeft+heightDown,target.down_right+heightDown-widthRight,target.down_left+widthLeft,target.down_right-widthRight,target.main_color);
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