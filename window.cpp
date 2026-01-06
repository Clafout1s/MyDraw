#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <math.h>
#include "vertexData.h"
#include "point.h"

const char* vertexSimpleCode = "#version 330 core\n"
    "layout(location=0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 vertColor;\n"
    "void main(){\n"
    "   vertColor = aColor;\n"
    "   gl_Position = vec4(aPos,1.0);\n"
    "};\n\0";

const char* fragmentSimpleCode = "#version 330 core\n"
    "out vec4 Color;\n"
    "in vec3 vertColor;\n"
    "void main(){\n"
    "   Color = vec4(vertColor,1.0);\n"
    "};\n\0";


int nbRects = 0;
int mouseState;
int screen_width=800;
int screen_height; // Defined by the width and ratio
float screen_ratio = 16/9.f;
float tile_size = 5;
float tile_ratio = 1;
vertexData windowVertices;
unsigned int MAIN_SC_VBO;
unsigned int MAIN_SC_EBO;

int sign(float num){
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

Point nearestTile(Point point,unsigned int size){
    // Takes arguments in pixel coordinates
    Point centerTemp = Point(0,0);
    if(std::fmod(point.x,size) == 0){
        centerTemp.x = abs(point.x - size/2.f);
    }
    if(std::fmod(point.y,size) == 0){
        centerTemp.y = abs(point.y - size/2.f);
    }
    if(centerTemp.x == 0){
        unsigned int line_square_x = abs(round(point.x/size)*size);
        centerTemp.x = size/2.f *sign(point.x-line_square_x) + (line_square_x);
    }
    if(centerTemp.y == 0){
        unsigned int line_square_y = abs(round(point.y/size)*size);
        centerTemp.y = size/2.f *sign(point.y-line_square_y) + (line_square_y);
    }

    return Point(centerTemp.x,centerTemp.y);
}

Point normalizePosition(Point point_global, float width, float height){
    float normx = 2*point_global.x/width -1;
    float normy = -(2*point_global.y/height -1);
    Point normalized = Point(normx,normy);
    return normalized;
}


void printCursor(GLFWwindow* window){
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    std::cout << "x: "<<x<<", y: "<<y<<"\n";
}


Rectangle calculateSquare(const Point& point,int width_pixel,float width_screen,float ratio){
    float width = 2* (float)width_pixel/width_screen; // normalize width
    float height = width*ratio;
    Rectangle square = Rectangle(point,width,height);
    return square;
}

void drawFigure(vertexData& figure_data, unsigned int VBO, unsigned int EBO){
    std::vector<float> data_list = figure_data.list();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data_list.size(), &data_list[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*figure_data.indices.size(),&figure_data.indices[0],GL_STATIC_DRAW);

}

void drawRect(vertexData& original_data ,Rectangle new_rect, unsigned int VBO, unsigned int EBO){
    // We consider that new_rect is in normalized coordinates
    std::vector<unsigned int> indices = {0,1,2,1,2,3};
    vertexData new_data= vertexData(new_rect,indices);
    original_data+= new_data;
    nbRects++;
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
    float denom_a = (end.x - start.x);
    float a = (end.y - start.y);
    if(denom_a != 0){
        a = a / denom_a;
    }
    float b = start.y - a * start.x;
    float length_x = end.x - start.x;
    float length_y = end.y - start.y;
    
    if(abs(length_x) >= abs(length_y)){
        int n_squares = abs(length_x) / tile_size ;
        for (size_t i = 1; i < n_squares; i++)
        {
            Point new_center_temp = Point();
            new_center_temp.x = start.x + (float)sign(length_x)*(i*tile_size); 
            new_center_temp.y = a * new_center_temp.x + b;
            Point new_center = nearestTile(new_center_temp,tile_size);
            
            new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
            drawRect(data,calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio),VBO,EBO);
        }
    }
    else{
        int n_squares = abs(length_y) / tile_size ;
        for (size_t i = 1; i < n_squares; i++)
        {
            Point new_center_temp = Point();
            new_center_temp.y = start.y + (float)sign(length_y)*(i*tile_size);
            new_center_temp.x = (new_center_temp.y - b)/a;
            Point new_center = nearestTile(new_center_temp,tile_size);
            new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
            
            drawRect(data,calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio),VBO,EBO);
        }
    }
    
}
Point last_point;
bool erase_mod = false;
bool last_clicked = false;

void processInput(GLFWwindow* window){

    if(mouseState==GLFW_PRESS){
        if(!erase_mod){
            // Draw
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            Point point = Point((float)x,(float)y);
            if(point.x >=0 && point.x <= screen_width && point.y>=0 && point.y <= screen_height){
                point = nearestTile(point,tile_size);
                Point pointNorm = normalizePosition(point,(float)screen_width,(float)screen_width/screen_ratio);
                
                if((last_clicked) && (abs(last_point.x - point.x)>1.5*tile_size || abs(last_point.y - point.y)>1.5*tile_size)){
                    //1.5 for floating point errors: points should always be separated by multiples of tile_size
                    drawLine(windowVertices,last_point,point,MAIN_SC_VBO,MAIN_SC_EBO);
                }
                last_point = point;
                last_clicked = true;
                Rectangle new_rect = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
                drawRect(windowVertices,new_rect,MAIN_SC_VBO, MAIN_SC_EBO);
            }
            else{
                last_clicked=false;
            }
        }
        
        else{
            // Erase
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            Point point = Point((float)x,(float)y);
            point = nearestTile(point,tile_size);
            Point pointNorm = normalizePosition(point,(float)screen_width,(float)screen_width/screen_ratio);
            Rectangle eraser = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
            bool added = windowVertices.eraseRectangles(eraser);
            
            nbRects=windowVertices.rects.size();
            drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
        }
    }
    else if(last_clicked){
        last_clicked=false;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        printf("Erase Mode\n");
        erase_mod = true;
    }
    else if(key == GLFW_KEY_C && action == GLFW_PRESS){
        printf("Clear\n");
        windowVertices.clear();
        nbRects = 0;
        drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
    }
    else if(key == GLFW_KEY_D && action == GLFW_PRESS){
        printf("Draw mode\n");
        erase_mod = false;
    }
    else if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS){
        printf("Grow size\n");
        tile_size *= 2;
    }
    else if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS){
        printf("Shrink size\n");
        if(tile_size/2 >= 1){
            tile_size /= 2;
        }
    }
    else if(key == GLFW_KEY_0 && action == GLFW_PRESS){
        printf("Test Rect\n");
        double x,y;
        glfwGetCursorPos(window,&x,&y);
        Point point = Point((float)x,(float)y);
        point = nearestTile(point,tile_size);
        Point pointNorm = normalizePosition(point,(float)screen_width,(float)screen_width/screen_ratio);
        Rectangle guess = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
        printRectGeogebra(guess);
        for (size_t i = 0; i < windowVertices.rects.size(); i++)
        {
            if(guess == windowVertices.rects[i]){
                printf("Exists !\n");
            }
        }
        

    }
}

int main(int argc, char const *argv[])
{
    // Setup

    srand(time(0));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    std::cout << std::fixed << std::setprecision(8); // To print more digits to float, for debugging purposes
    // Window
    screen_height = (int)(screen_width/screen_ratio);
    GLFWwindow* app_window =  glfwCreateWindow(screen_width,screen_height,"MyDraw",NULL,NULL);
    
    if(app_window == NULL){
        std::cout << "Error during window creation."<<"\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(app_window);
    //glfwSetWindowAspectRatio(app_window,screen_ratio,1);    

    glfwSetFramebufferSizeCallback(app_window,windowResizeCallback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Error during GLAD setup." <<"\n";
        return -1;
    }

    glfwSetInputMode(app_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetKeyCallback(app_window, key_callback);
    glfwSetInputMode(app_window, GLFW_STICKY_KEYS, GLFW_TRUE);

    unsigned int shaderProgram = loadShaders(vertexSimpleCode,fragmentSimpleCode);

    glGenBuffers(1,&MAIN_SC_VBO);

    glGenBuffers(1,&MAIN_SC_EBO);
    
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, MAIN_SC_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,MAIN_SC_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,0,0,GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, MAIN_SC_VBO);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2* sizeof(float)));
    glEnableVertexAttribArray(1);

    int count = 2;    

    while(!glfwWindowShouldClose(app_window)){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        mouseState = glfwGetMouseButton(app_window, GLFW_MOUSE_BUTTON_LEFT);
        processInput(app_window);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        
        
        glDrawElements(GL_TRIANGLES,nbRects*8,GL_UNSIGNED_INT,0); //number 2: total number of vertices
        glBindVertexArray(0);

        // Necessary for any window
        glfwPollEvents();
        glfwSwapBuffers(app_window);
    }

    //free(vertices);
    glfwTerminate();
    return 0;
}
