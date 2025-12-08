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
         * Only works for squares of the same size.
         * If none is found, returns -1
         */
        int search_perfect(Point& point){
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

        /**
         * Returns the indices of all the squares that are included into this one, in their apparition order.
         * Only works for squares of the same size.
         * If none is found, returns -1
         */
        std::vector<unsigned int> search_all_included(Square& big){
            std::vector<unsigned int> found = {};
            for (size_t i = 0; i < squares.size(); i++)
            {
                if(big.does_include(squares[i])){
                    printf("big\n");
                    printPoint(big.top_left);
                    printPoint(big.top_right);
                    printPoint(big.down_left);
                    printPoint(big.down_right);
                    printf("small\n");
                    printPoint(squares[i].top_left);
                    printPoint(squares[i].top_right);
                    printPoint(squares[i].down_left);
                    printPoint(squares[i].down_right);
                    printf("--------------\n\n");
                    found.insert(found.end(),i);
                }
            }
            
            return found;
        }

        size_t delete_squares(Square& eraser,int size){
            printf("delete\n");
            std::vector<unsigned int> delete_list = this->search_all_included(eraser);
            size_t deleted = 0;
            for (size_t i = 0; i < delete_list.size(); i++)
            {

                this->squares.erase(this->squares.begin()+delete_list[i]+deleted); 
                // deleted is to account for the squares list changing its indices when deleting an element
                // it can ONLY be done like this because delete_list is in the same order as the apparition of the corresponding squares.
                deleted++;

            }
            return deleted;
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
