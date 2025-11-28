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

        /**
         * Returns the index of the first square from which its center point is equals to point.
         * If none is found, returns -1
         */
        int search(Point& point){
            size_t i = 0;
            while (i < this->squares.size())
            {
                if(this->squares[i].center_p == point){
                    return i;
                }
                i++;
            }
            return -1;
        }

        bool delete_square(Point& point){
            int i_del = this->search(point);
            if(i_del >= 0){
                this->squares.erase(this->squares.begin()+i_del);
                return true;
                
            }
            return false;
        }
        
        vertexData& clear(){
            this->squares.clear();
            this->indices.clear();
            return *this;
        }

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
#endif
