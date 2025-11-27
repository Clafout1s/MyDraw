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
int screen_height; // Defined by the width and ratio
float screen_ratio = 1.f;
double tile_size = 5;

std::vector<double> vertex_list;


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
    
    int old_width = screen_width;
    screen_width = width;
    screen_height = height;
    // Ratio should be conserved unless fullscreen

    tile_size *= (screen_width/(float)old_width);

}

Point nearestTile(Point point,unsigned int tile_size){
    // Takes arguments in pixel coordinates
    Point square = Point(0,0);
    if(std::fmod(point.x,tile_size) == 0){
        printf("%lf, in line x\n",point.x);
        square.x = abs(point.x - tile_size/2.f);
    }
    if(std::fmod(point.y,tile_size) == 0){
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


Square calculateSquare(const Point& point,int width_pixel,float width_screen,float ratio){
    float width = 2* width_pixel/width_screen; // normalize width
    float height = width*ratio;

    Square square = Square(point,width,height);
    

    return square;
}

void drawFigure(vertexData& figure_data, unsigned int VBO, unsigned int EBO){
    std::vector<double> data_list = figure_data.list();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double)*data_list.size(), &data_list[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*figure_data.indices.size(),&figure_data.indices[0],GL_STATIC_DRAW);

}

void drawSquare(vertexData& original_data ,Square new_square, unsigned int VBO, unsigned int EBO){
    // We consider that new_square is in normalized coordinates
    std::vector<unsigned int> indices = {0,1,2,1,2,3};
    vertexData new_data= vertexData(new_square,indices);
    original_data+= new_data;
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

void drawLine(vertexData& data, Point& start, Point& end,unsigned int VBO, unsigned int EBO){
    // We consider that start and end are in pixel coordinates
    double denom_a = (end.x - start.x);
    double a = (end.y - start.y);
    if(denom_a != 0){
        a = a / denom_a;
    }
    double b = start.y - a * start.x;
    double length_x = end.x - start.x;
    double length_y = end.y - start.y;
    printf("start: %f %f; end: %f %f\n",start.x,start.y,end.x,end.y);
    printf("a: %f b: %f\n",a,b);
    
    if(abs(length_x) >= abs(length_y)){
        int n_squares = abs(length_x) / tile_size ;
        printf("n_squares: %d\n",n_squares);
        for (size_t i = 1; i < n_squares; i++)
        {
            Point new_center = Point();
            new_center.x = start.x + (double)sign(length_x)*(i*tile_size); 
            new_center.y = a * new_center.x + b;
            printPoint(new_center);
            new_center = nearestTile(new_center,tile_size);
            
            new_center = normalizePosition(new_center,(double)screen_width,(double)screen_width/screen_ratio);
            //printPoint(new_center);
            drawSquare(data,calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio),VBO,EBO);
        }
    }
    else{
        //printf("%f\n",length_y_temp);
        int n_squares = abs(length_y) / tile_size ;
        printf("n_squares: %d\n",n_squares);
        for (size_t i = 1; i < n_squares; i++)
        {
            Point new_center = Point();
            new_center.y = start.y + (double)sign(length_y)*(i*tile_size);
            new_center.x = (new_center.y - b)/a;
            new_center = nearestTile(new_center,tile_size);
            printPoint(new_center);
            new_center = normalizePosition(new_center,(double)screen_width,(double)screen_width/screen_ratio);
            
            drawSquare(data,calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio),VBO,EBO);
        }
    }
    
}

Point last_point;
bool last_pressed = false;

void processInput(GLFWwindow* window,vertexData& vertexDataObject, unsigned int VBO, unsigned int EBO){

    if(mouseState==GLFW_PRESS){
        Point point = Point(0,0);
        glfwGetCursorPos(window,&point.x,&point.y);
        if(point.x >=0 && point.x <= screen_width && point.y>=0 && point.y <= screen_height){
            point = nearestTile(point,tile_size);
            Point pointNorm = normalizePosition(point,(double)screen_width,(double)screen_width/screen_ratio);
            
            if((last_pressed) && (abs(last_point.x - point.x)>1.5*tile_size || abs(last_point.y - point.y)>1.5*tile_size)){
                //1.5 for floating point errors: points should always be separated by multiples of tile_size
                printf("Print a line\n");
                drawLine(vertexDataObject,last_point,point,VBO,EBO);
            }
            last_point = point;
            last_pressed = true;
            drawSquare(vertexDataObject,calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio),VBO, EBO);
        }
    }
    else if(last_pressed){
        last_pressed=false;
    }
}

int main(int argc, char const *argv[])
{
    // Setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Window
    screen_height = (int)(screen_width/screen_ratio);
    GLFWwindow* app_window =  glfwCreateWindow(screen_width,screen_height,"MyDraw",NULL,NULL);
    
    if(app_window == NULL){
        std::cout << "Error during window creation."<<"\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(app_window);
    glfwSetWindowAspectRatio(app_window,1,1);    

    glfwSetFramebufferSizeCallback(app_window,windowResizeCallback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Error during GLAD setup." <<"\n";
        return -1;
    }

    glfwSetInputMode(app_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    unsigned int shaderProgram = loadShaders(vertexSimpleCode,fragmentSimpleCode);

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
    glVertexAttribPointer(0,2,GL_DOUBLE,GL_FALSE,2*sizeof(double),(void*)0);
    glEnableVertexAttribArray(0);

    int count = 2;    

    vertexData vertest;

    while(!glfwWindowShouldClose(app_window)){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        mouseState = glfwGetMouseButton(app_window, GLFW_MOUSE_BUTTON_LEFT);
        processInput(app_window,verts,VBO,EBO);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        
        
        glDrawElements(GL_TRIANGLES,nbSquares*8,GL_UNSIGNED_INT,0); //number 2: total number of vertices
        glBindVertexArray(0);

        // Necessary for any window
        glfwPollEvents();
        glfwSwapBuffers(app_window);
    }

    //free(vertices);
    glfwTerminate();
    return 0;
}
