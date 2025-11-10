#ifndef MY_VERTEX_DATA_H_
#define MY_VERTEX_DATA_H_

#include <vector>
#include <algorithm>
#include "point.h"

class vertexData{
    public:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        vertexData(){
        }
        vertexData(std::vector<float> v, std::vector<unsigned int> i): vertices(v), indices(i){}

        vertexData& operator+=(const vertexData& v2){
            if(this->vertices.empty() && this->indices.empty()){
                this->vertices = v2.vertices;
                this->indices = v2.indices;
                return *this;
            }
            /*
            bool identical = true;
            std::cout << "size: " << v2.vertices.size() << "\n";
            for(int i=0;i<v2.vertices.size();i++){
                std::cout << "point: " <<v2.vertices[i] <<"\n";
                if((std::find(vertices.begin(),vertices.end(),v2.vertices[i]))== vertices.end()){
                    std::cout << "different\n";
                    identical = false;
                }
                
            }
            if(identical){
                return *this;
            }*/
            for (size_t i = 0; i < v2.vertices.size(); i++)
            {
                vertices.push_back(v2.vertices[i]);
            }
            
            unsigned int old_max_i = indices[indices.size()-1];
            for(size_t i=0;i<v2.indices.size();i++){
                indices.push_back(v2.indices[i] + 1 + old_max_i);
            }

            return *this;
        };
        
};

inline void printVertexData(const vertexData& v){
    std::cout << "Vertices: \n";
    for (size_t i = 0; i < v.vertices.size(); i++)
    {
        if(i+1 %3==0){
            std::cout << "\n";
        }
        std::cout << v.vertices[i];
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

#endif
