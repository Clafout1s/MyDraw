#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "vertexData.h"

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
void windowResizeCallback(GLFWwindow* window,int width, int height){
    glViewport(0,0,width,height);
}

void normalizePosition(double& xn, double& yn,double pointX, double pointY, double width, double height){
    // Warning: this function changes the values of xp and xy, be wary !
    xn = 2*pointX/width -1;
    yn = -(2*pointY/height -1);
}


void printCursor(GLFWwindow* window){
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    std::cout << "x: "<<x<<", y: "<<y<<"\n";
}


std::vector<float> calculateSquare(std::vector<float> point,int width_pixel,float width_screen,float ratio){
    // Only the first two values inside point are evaluated, the rest is ignored.
    float px = point[0];
    float py = point[1];
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
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS){
        double xp,yp,xn,yn; // p: pixel, n: normalized
        int width,height; // window dimensions, in pixels
        glfwGetWindowSize(window,&width,&height);
        float ratio = width/float(height);
        
        glfwGetCursorPos(window,&xp,&yp);
        normalizePosition(xn,yn,xp,yp,(double)width,(double)height);
        std::vector<float> points = {(float)xn,(float)yn};
        drawSquare(vertexDataObject,calculateSquare(points,10,(float)width,ratio),VBO, EBO);
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
    int width=800;
    float ratio = 8/6.f;
    GLFWwindow* app_window =  glfwCreateWindow(width,(int)(width/ratio),"MyDraw",NULL,NULL);
    
    if(app_window == NULL){
        std::cout << "Error during window creation."<<"\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(app_window);
    glfwSetFramebufferSizeCallback(app_window,windowResizeCallback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Error during GLAD setup." <<"\n";
        return -1;
    }

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
