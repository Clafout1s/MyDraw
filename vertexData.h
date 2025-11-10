#ifndef MY_VERTEX_DATA_H_
#define MY_VERTEX_DATA_H_

#include <vector>
#include <algorithm>
#include "point.h"
#include "square.h"

class vertexData{
    public:
        std::vector<Square> squares;
        std::vector<unsigned int> indices;

        vertexData(){
        }
        vertexData(Square s, std::vector<unsigned int> i): indices(i){
            squares.insert(squares.begin(),s);
        }
        vertexData(std::vector<Square> s_list, std::vector<unsigned int> i): squares(s_list), indices(i){}

        vertexData& operator+=(const vertexData& v2){
            if(this->squares.empty() && this->indices.empty()){
                this->squares = v2.squares;
                this->indices = v2.indices;
                return *this;
            }
            
            if(std::find(squares.begin(),squares.end(),v2.squares[0])!=squares.end()){
                return *this;
            }
            

            for (size_t i = 0; i < v2.squares.size(); i++)
            {
                squares.push_back(v2.squares[i]);
            }
            
            unsigned int old_max_i = indices[indices.size()-1];
            for(size_t i=0;i<v2.indices.size();i++){
                indices.push_back(v2.indices[i] + 1 + old_max_i);
            }

            return *this;
        };

        std::vector<double> list(){
            std::vector<double> vec;

            for (size_t i = 0; i < squares.size(); i++)
            {
                std::vector<double> sq_list = squares[i].list();
                vec.insert(vec.end(),sq_list.begin(),sq_list.end());
            }
            return vec;
        }
        
};

/*
inline void printVertexData(const vertexData& v){
    std::cout << "Vertices: \n";
    for (size_t i = 0; i < v.squares.size(); i++)
    {
        if(i+1 %3==0){
            std::cout << "\n";
        }
        std::cout << v.squares[i];
        if(i+1 < v.vertices.size()){
            std::cout <<", ";
        }
    }
    std::cout << "\n";

    std::cout << "Indices: \n";
    for (size_t i = 0; i < v.indices.size(); i++)
    {
        if(i+1 %3==0){
            std::cout << "\n";
        }
        std::cout << v.indices[i];
        if(i+1 < v.indices.size()){
            std::cout <<", ";
        }
    }
    std::cout << "\n";
}
*/
#endif
