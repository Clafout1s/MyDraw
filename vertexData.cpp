#include <stdlib.h>
#include <iostream>
#include "vertexData.h"

vertexData* vertexDataCreate(std::vector<float> vertices, std::vector<unsigned int> indices){
    vertexData* data = (vertexData*)malloc(200);
    std::cout << sizeof(data) <<"\n";
    
    data->indices = indices;
    data->vertices = vertices;
    std::cout << "creation "<< data->vertices[0]<<"\n";
    return data;
}

void vertexDataDestroy(vertexData* data){
    free(data);
}

void vertexDataAdd(vertexData* data, std::vector<float> vertices, std::vector<unsigned int> indices){
    data->vertices.insert(data->vertices.end(),vertices.begin(),vertices.end());
    unsigned int old_max_i = data->indices[-1];
    data->indices.insert(data->indices.end(),indices.begin(),indices.end());
    for (unsigned int new_i=old_max_i; new_i < indices.size(); new_i++)
    {
        data->indices[new_i]+= 1+old_max_i;
    }
    
}