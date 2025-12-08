#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "point.h"
#include "square.h"

int main()
{
    Square eraser = Square(Point(0,0,0),50);
    Square small = Square(Point(0,0+25),20);
    printSquare(eraser);
    printSquare(small);
    return 0;
}
