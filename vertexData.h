#ifndef MY_VERTEX_DATA_H_
#define MY_VERTEX_DATA_H_

#include <vector>
#include <algorithm>
#include "point.h"
#include "square.h"

class vertexData{
    public:
        std::vector<Rectangle> rects;
        std::vector<unsigned int> indices;

        vertexData(){
        }
        vertexData(Rectangle s, std::vector<unsigned int> i): indices(i){
            rects.insert(rects.begin(),s);
        }
        vertexData(std::vector<Rectangle> s_list, std::vector<unsigned int> i): rects(s_list), indices(i){}

        vertexData& operator+=(const vertexData& v2){
            if(this->rects.empty() && this->indices.empty()){
                this->rects = v2.rects;
                this->indices = v2.indices;
                return *this;
            }
            
            if(std::find(rects.begin(),rects.end(),v2.rects[0])!=rects.end()){
                return *this;
            }
            

            for (size_t i = 0; i < v2.rects.size(); i++)
            {
                rects.push_back(v2.rects[i]);
            }
            
            unsigned int old_max_i = indices[indices.size()-1];
            for(size_t i=0;i<v2.indices.size();i++){
                indices.push_back(v2.indices[i] + 1 + old_max_i);
            }

            return *this;
        };
        

        
        vertexData& clear(){
            this->rects.clear();
            this->indices.clear();
            return *this;
        }

        std::vector<double> list(){
            std::vector<double> vec;

            for (size_t i = 0; i < rects.size(); i++)
            {
                std::vector<double> sq_list = rects[i].list();
                vec.insert(vec.end(),sq_list.begin(),sq_list.end());
            }
            return vec;
        }
        
};
#endif
