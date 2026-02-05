/*
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
}*/


/**
     * Cuts a rectangle into squares and eventually the rest into one smaller rectangle.
     * Returns a tuple with two values: 
     * the first one is a vector of squares, the second a rectangle.
     */
/*
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
*/