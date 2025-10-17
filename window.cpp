#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>
#include "vertexData.h"
#include "point.h"

const char* vertexSimpleCode = "#version 330 core\n"
    "layout(location=0) in vec3 aPos;\n"
    "void main(){\n"
    "   gl_Position = vec4(aPos,1.0);"
    "};\n\0";

const char* fragmentSimpleCode = "#version 330 core\n"
    "out vec4 Color;\n"
    "void main(){\n"
    "   Color = vec4(1.0f,0.9f,0.2f,1.0f);\n"
    "};\n\0";


int nbSquares = 0;
int mouseState;
int screen_width=800;
float screen_ratio = 16/9.f;
int tile_size = 20;

int sign(double num){
    if(num<0){
        return -1;
    }
    else if(num>0){
        return 1;
    }
    else{
        return 0;
    }
}

void windowResizeCallback(GLFWwindow* window,int width, int height){
    glViewport(0,0,width,height);
}

Point nearestTile(Point point,unsigned int tile_size){
    Point square = Point(0,0);
    if(std::fmod(point.x,tile_size) == 0){
        printf("%lf, in line x\n",point.x);
        square.x = abs(point.x - tile_size/2.f);
    }
    if(std::fmod(point.x,tile_size) == 0){
        printf("%lf, in line y\n",point.y);
        square.y = abs(point.y - tile_size/2.f);
    }
    if(square.x == 0){
        unsigned int line_square_x = abs(round(point.x/tile_size)*tile_size);
        square.x = tile_size/2.f *sign(point.x-line_square_x) + (line_square_x);
    }
    if(square.y == 0){
        unsigned int line_square_y = abs(round(point.y/tile_size)*tile_size);
        square.y = tile_size/2.f *sign(point.y-line_square_y) + (line_square_y);
    }

    return square;
}

Point normalizePosition(Point point_global, double width, double height){
    // Warning: this function changes the values of xn and yn, be wary !
    Point normalized = Point(point_global.x,point_global.y);
    normalized.x = 2*normalized.x/width -1;
    normalized.y = -(2*normalized.y/height -1);
    return normalized;
}


void printCursor(GLFWwindow* window){
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    std::cout << "x: "<<x<<", y: "<<y<<"\n";
}


std::vector<float> calculateSquare(Point point,int width_pixel,float width_screen,float ratio){
    float px = point.x;
    float py = point.y;
    float width = 2* width_pixel/width_screen /2; // normalize width
    float height = width*ratio;

    std::vector<float> vertices= {
        px-(width/2), py+(height/2), 0.f,
        px+(width/2), py+(height/2), 0.f,
        px-(width/2), py-(height/2), 0.f,
        px+(width/2), py-(height/2), 0.f
 
    };
    
    return vertices;
}

void drawFigure(const vertexData& figure_data, unsigned int VBO, unsigned int EBO){
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*figure_data.vertices.size(), &figure_data.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*figure_data.indices.size(),&figure_data.indices[0],GL_STATIC_DRAW);
}

void drawSquare(vertexData& original_data ,std::vector<float> new_vertices, unsigned int VBO, unsigned int EBO){
    std::vector<unsigned int> indices = {0,1,2,1,2,3};
    vertexData* new_data= new vertexData(new_vertices,indices);
    original_data+= *new_data;
    nbSquares++;
    drawFigure(original_data,VBO,EBO);
    
}

void checkCompileErrors(unsigned int shader, std::string name){
    int success;
    if(name != "PROGRAM"){
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
        if(! success){
            std::cout << "Error during compilation of shader " <<name<<"\n";
        }
    }
    else{
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success){
            std::cout << "Error during linking of program"<<"\n";
        }
    }
}

unsigned int loadShaders(const char* vertexShaderCode, const char* fragmentShaderCode){
    
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);

    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
        
    unsigned int shaderFinal = glCreateProgram();
    glAttachShader(shaderFinal, vertex);
    glAttachShader(shaderFinal, fragment);
    glLinkProgram(shaderFinal);
    checkCompileErrors(shaderFinal, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return shaderFinal;
}

void processInput(GLFWwindow* window,vertexData& vertexDataObject, unsigned int VBO, unsigned int EBO){
    if(mouseState==GLFW_PRESS){
        int screen_height;
        glfwGetWindowSize(window,&screen_width,&screen_height);
        Point point = Point(0,0);
        glfwGetCursorPos(window,&point.x,&point.y);
        if(point.x >=0 && point.x <= screen_width && point.y>=0 && point.y <= screen_height){
            point = nearestTile(point,20);
            point = normalizePosition(point,(double)screen_width,(double)screen_width/screen_ratio);
            drawSquare(vertexDataObject,calculateSquare(point,40,(float)screen_width,screen_ratio),VBO, EBO);
        }
        
    }
}

int main(int argc, char const *argv[])
{
    // Setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Window

    GLFWwindow* app_window =  glfwCreateWindow(screen_width,(int)(screen_width/screen_ratio),"MyDraw",NULL,NULL);
    
    if(app_window == NULL){
        std::cout << "Error during window creation."<<"\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(app_window);
    glfwSetWindowAspectRatio(app_window,16,9);

    glfwSetFramebufferSizeCallback(app_window,windowResizeCallback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Error during GLAD setup." <<"\n";
        return -1;
    }

    glfwSetInputMode(app_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    unsigned int shaderProgram = loadShaders(vertexSimpleCode,fragmentSimpleCode);

    std::vector<float> position = {-0.5,0.1};
    std::vector<float> position2 = {0.8,0.2};
    std::vector<float> position3 = {-0.7,-0.8};
    std::vector<float> position4 = {-2.0,-1.0};
    
    std::vector<unsigned int> indices = {
        0,1,2,1,2,3
    };

    vertexData verts;

    unsigned int VBO;
    glGenBuffers(1,&VBO);

    unsigned int EBO;
    glGenBuffers(1,&EBO);
    
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,0,0,GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    int count = 2;    


    while(!glfwWindowShouldClose(app_window)){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        mouseState = glfwGetMouseButton(app_window, GLFW_MOUSE_BUTTON_LEFT);
        processInput(app_window,verts,VBO,EBO);
        
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        
        glDrawElements(GL_TRIANGLES,nbSquares*2*3,GL_UNSIGNED_INT,0); //number 2: total number of vertices
        glBindVertexArray(0);

        // Necessary for any window
        glfwPollEvents();
        glfwSwapBuffers(app_window);
    }

    //free(vertices);
    glfwTerminate();
    return 0;
}
