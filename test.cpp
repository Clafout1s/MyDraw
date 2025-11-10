#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "point.h"
#include "square.h"

int main()
{
    Point p1 = Point(1,1);
    Point p2 = Point(2,1);
    Point p3 = Point(1,2);
    Point p4 = Point(2,2);

    Point pa = Point(1,1);
    Point pb = Point(2,2);
    Point pc = Point(1,2);
    Point pd = Point(2,2);

    Square s1 = Square(p1,p2,p3,p4);
    Square s2 = Square(pa,pb,pc,pd);

    bool res = s1==s2;
    std::cout << std::boolalpha;
    std::cout << res <<"\n";

    std::vector<double> list = s1.list(); 
    for (size_t i = 0; i < list.size(); i++)
    {
        std::cout << list[i]<<" ";
    }
    std::cout << ";\n";

    return 0;
}
