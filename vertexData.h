#ifndef MY_VERTEX_DATA_H_
#define MY_VERTEX_DATA_H_

#include <vector>
#include <algorithm>
#include "rectangle.h"

class vertexData{
    public:
        std::vector<Rectangle> rects;
        std::vector<unsigned int> indices;

        vertexData(){}
        vertexData(Rectangle rec):vertexData(rec,std::vector<unsigned int>{0,1,2,1,2,3}){}
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
                //eraseRectangles((Rectangle&)v2.rects[i]);
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

        std::vector<float> list(){
            std::vector<float> vec;

            for (size_t i = 0; i < rects.size(); i++)
            {
                std::vector<float> sq_list = rects[i].list(0,0,0);
                vec.insert(vec.end(),sq_list.begin(),sq_list.end());
            }
            return vec;
        }

        bool eraseOneRectangle(Rectangle& eraser){
            size_t i = 0;
            bool found = false;
            while (!false && i<rects.size())
            {
                if(rects[i]==eraser){
                    rects.erase(rects.begin()+i);
                    indices.erase(indices.end()-6,indices.end());
                    found = true;
                }
                i++;
            }
            return found;
        }
        
        bool eraseRectangles(Rectangle& eraser){
            std::vector<int> erase_list = {};
            std::vector<Rectangle> add_list = {};
            for (size_t i = 0; i < rects.size(); i++)
            {
                std::vector<Rectangle> result = cutFromRectangle(rects[i],eraser);
                if(!result.empty()){
                    erase_list.insert(erase_list.end(),i);
                    for (size_t y = 0; y < result.size(); y++){

                        if(!result[y].isNull()){
                           add_list.insert(add_list.end(),result[y]);
                        }
                        
                    }
                        
                }
            }
            std::sort(erase_list.begin(),erase_list.end(),std::greater<int>()); // Sort descending, to avoid issues in deletion indexes
            
            for (size_t e = 0; e < erase_list.size(); e++)
            {
                rects.erase(rects.begin()+erase_list[e]);
                indices.erase(indices.end()-6,indices.end());
            }
            for (size_t a = 0; a < add_list.size(); a++)
            {
                std::vector<unsigned int> indices_new = {0,1,2,1,2,3};
                *this+=vertexData(add_list[a],indices_new);
            }
            return !erase_list.empty() && !add_list.empty();
        }
        
};
#endif
