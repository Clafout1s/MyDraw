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
                std::vector<float> sq_list = rects[i].list();
                vec.insert(vec.end(),sq_list.begin(),sq_list.end());
            }
            return vec;
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
                            /*
                            std::cout << "\nRec shouldn't be NULL !\n Printing eraser, the inter and rect: \n";
                            printRectGeogebra(eraser);
                            printRectGeogebra(result[y]);
                            printRectGeogebra(rects[i]);
                            throw std::logic_error("");
                            */
                           add_list.insert(add_list.end(),result[y]);
                        }
                        
                    }
                        
                }
            }
            printf("Erase: \n");
            for (size_t e = 0; e < erase_list.size(); e++)
            {
                printRectGeogebra(rects[erase_list[e]]);
                rects.erase(rects.begin()+erase_list[e]);
                indices.erase(indices.end()-6,indices.end());
            }
            printf("Add: \n");
            for (size_t a = 0; a < add_list.size(); a++)
            {
                printRectGeogebra(add_list[a]);
                std::vector<unsigned int> indices_new = {0,1,2,1,2,3};
                *this+=vertexData(add_list[a],indices_new);
            }
            printf("-----\n");
            return !erase_list.empty() && !add_list.empty();
        }
        
};
#endif
