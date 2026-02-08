#ifndef MY_VERTEX_DATA_H_
#define MY_VERTEX_DATA_H_

#include <vector>
#include <algorithm>
#include <cassert>
#include "rectangle.h"

/**
 * Stores a list of rectangles, and a list of indices, to be fed into OpenGL for rendering.
 * Technically it can enter different sets of indices together, but it is untested behavior.
 */
class vertexData{

    public:
        std::vector<Rectangle> rects;
        std::vector<unsigned int> indices;
        
        vertexData(){}

        /**
         * Creates a vertexData with {0,1,2,1,2,3} as its indices (the base indices for a rectangle). 
         */
        vertexData(Rectangle rec):vertexData(rec,std::vector<unsigned int>{0,1,2,1,2,3}){}

        /**
         * Creates a vertexData with s as its sole rectangle, and i as its indices. 
         */
        vertexData(Rectangle r, std::vector<unsigned int> i): indices(i){
            rects.insert(rects.begin(),r);
        }

        /**
         * Creates a vertexData with a starting list of rectangles, and automatically generates its indices from the base 
         * indices given in argument.
         * The argument indices_base are the indices of only one Rectangle.
         */
        vertexData(std::vector<Rectangle> s_list, std::vector<unsigned int> indices_base): rects(s_list){
            unsigned int old_index_max_value=0;
            for (size_t i = 0; i < s_list.size(); i++)
            {
                for(size_t y=0;y<indices_base.size();y++){
                    indices.push_back(indices_base[y] + 1 + old_index_max_value);
                }
                old_index_max_value = indices[indices.size()-1];
            }
        }

        /**
         * Creates a vertexData with a starting list of rectangles, and automatically generates its indices from the base 
         * indices {0,1,2,1,2,3}.
         */
        vertexData(std::vector<Rectangle> s_list): rects(s_list){
            std::vector<unsigned int> indices_one_rec = {0,1,2,1,2,3};
            unsigned int old_index_max_value=0;
            for (size_t i = 0; i < s_list.size(); i++)
            {
                for(size_t y=0;y<indices_one_rec.size();y++){
                    indices.push_back(indices_one_rec[y] + 1 + old_index_max_value);
                }
                old_index_max_value = indices[indices.size()-1];
            }
        }

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
                //eraseRectangles((Rectangle&)v2.rects[i]); // Slower but cooler
                rects.push_back(v2.rects[i]);
            }
            unsigned int old_index_max_value = indices[indices.size()-1];
            for(size_t y=0;y<v2.indices.size();y++){
                indices.push_back(v2.indices[y] + 1 + old_index_max_value);
            }
            return *this;
        };

        /**
         * An alternative to +=, that considers the positions of where to insert the rectangles.
         * The indices are still added at the end.
         */
        vertexData& addAtPosition(const vertexData& v2,std::vector<unsigned int> positions){
            assert(positions.size()==v2.rects.size());
            if(this->rects.empty() && this->indices.empty()){
                this->rects = v2.rects;
                this->indices = v2.indices;
                return *this;
            }
            
            std::tuple<std::vector<Rectangle>,std::vector<unsigned int>> sorted_arrays = sortRectArrayByPosition(v2.rects,positions);
            std::vector<Rectangle> v2_rects_sorted = std::get<0>(sorted_arrays);
            std::vector<unsigned int> positions_sorted = std::get<1>(sorted_arrays);
            std::vector<unsigned int> new_indices = {0,1,2,1,2,3};
            unsigned int old_index_max_value=0;
            for (size_t i = 0; i < v2.rects.size(); i++)
            {
                this->rects.insert(this->rects.begin()+positions_sorted[i],v2_rects_sorted[i]);

                old_index_max_value = indices[indices.size()-1];
                for(size_t y=0;y<new_indices.size();y++){
                    indices.push_back(new_indices[y] + 1 + old_index_max_value);
                }
            }

            return *this;
        }

        /**
         * Resets the vertexData to emptiness.
         */
        vertexData& clear(){
            this->rects.clear();
            this->indices.clear();
            return *this;
        }

        /**
         * Lists the data of the Rectangles to be usable by OpenGL.
         */
        std::vector<float> list(){
            std::vector<float> vec;

            for (size_t i = 0; i < rects.size(); i++)
            {
                std::vector<float> sq_list = rects[i].list();
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
        
        /**
         * Erase all rectangles in the vertexData that are under the eraser (and recuts them when needed).
         */
        bool eraseRectangles(Rectangle& eraser){
            std::vector<int> erase_list = {};
            std::vector<Rectangle> add_list = {};
            std::vector<unsigned int> add_list_positions = {};
            for (size_t i = 0; i < rects.size(); i++)
            {
                std::vector<Rectangle> result = cutFromRectangle(rects[i],eraser);
                if(!result.empty()){
                    erase_list.insert(erase_list.end(),i);
                    for (size_t y = 0; y < result.size(); y++){

                        if(!result[y].isNull()){
                           add_list.insert(add_list.end(),result[y]);
                           add_list_positions.insert(add_list_positions.end(),i);
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

            vertexData new_rects_data = vertexData(add_list);
            if(!add_list.empty()) this->addAtPosition(new_rects_data,add_list_positions);
 
            return !erase_list.empty() && !add_list.empty();
        }


    private:
        /** 
         * Sorts vec_rects by sorting positions.
         * Both vectors should have the same length, as all of their contents are considered linked by their indexes.
        */
        std::tuple<std::vector<Rectangle>,std::vector<unsigned int>> sortRectArrayByPosition(const std::vector<Rectangle> vec_rects, const std::vector<unsigned int> positions){
            std::vector<Rectangle> vec_rects_copy = vec_rects;
            std::vector<Rectangle> new_vec_rects={};
            std::vector<unsigned int> positions_copy = positions;
            std::vector<unsigned int> new_positions = {};
            
            size_t smallest_pos_i = 0;
            for (size_t y = 0; y < positions.size(); y++)
            {
               for (size_t i = 1; i < positions_copy.size(); i++)
                {
                    if(positions_copy[i]<positions_copy[smallest_pos_i]){
                        smallest_pos_i = i;
                    }
                }
                new_positions.insert(new_positions.end(),positions_copy[smallest_pos_i]);
                new_vec_rects.insert(new_vec_rects.end(),vec_rects_copy[smallest_pos_i]);
                positions_copy.erase(positions_copy.begin()+smallest_pos_i);
                vec_rects_copy.erase(vec_rects_copy.begin()+smallest_pos_i);
                smallest_pos_i = 0;
            }
            
            std::tuple<std::vector<Rectangle>,std::vector<unsigned int>> result ={
                new_vec_rects,new_positions
            };
            return result;
        }
};
#endif
