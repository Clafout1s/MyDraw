#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <math.h>
#include <cmath>
#include <regex>
#include "vertexData.h"

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


const float MINIMUM_TILE_SIZE = 10;
const float MAXIMUM_TILE_SIZE = MINIMUM_TILE_SIZE*10;
float tile_size = MINIMUM_TILE_SIZE;

vertexData windowVertices;
unsigned int MAIN_SC_VBO;
unsigned int MAIN_SC_EBO;

Rectangle borderIndicator;
unsigned int UI_VBO;
unsigned int UI_EBO;

Color active_color={0,0,0};

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
float nearestCoordinate(float old_coord,unsigned int size){
    float new_coord = 0;
    if(std::fmod(old_coord,size) == 0){
        new_coord = abs(old_coord - size/2.f);
    }
    if(new_coord == 0){
        unsigned int line_square_coord = abs(round(old_coord/size)*size);
        new_coord= size/2.f *sign(old_coord-line_square_coord) + (line_square_coord);
    }
    return new_coord;
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

Rectangle calculateRectangle(const Point& point,int width_pixel,float height_pixel,float width_screen,float ratio){
    float width = 2* (float)width_pixel/width_screen; // normalize width
    float height = 2* (float)height_pixel/width_screen*ratio;
    Rectangle square = Rectangle(point,width,height,active_color);
    return square;
}

Rectangle calculateSquare(const Point& point,int width_pixel,float width_screen,float ratio){
    float width = 2* (float)width_pixel/width_screen; // normalize width
    float height = width*ratio;
    Rectangle square = Rectangle(point,width,height,active_color);
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
/**
 * We consider that start and end are in pixel coordinates.
 */
std::vector<Rectangle> findLine(vertexData& data, Point& start, Point& end, float tile_size){

    std::vector<Rectangle> line_rects = {};
    float length_x = abs(end.x - start.x);
    float length_y = abs(end.y - start.y);
    Point direction = Point(sign(end.x-start.x),sign(end.y-start.y));
    bool horizontal = length_x>=length_y;

    // prim: largest axis of the two: x if length x is bigger than length y
    // sec: smallest axis of the two: y if length x is smaller than length y
    float length_prim = horizontal? length_x: length_y;
    float length_sec = horizontal? length_y: length_x;
    float width_line_prim; 
    float step_sec; 
    float nb_lines_sec;

    nb_lines_sec = 2* floor((length_sec+tile_size)/(float)tile_size)-1;

    // Corrections for two and one line
    if(nb_lines_sec == 1 && length_sec != 0) nb_lines_sec = 2;
    if(nb_lines_sec == 1 && length_sec == 0){
        // one line step
        nb_lines_sec=1;
        step_sec = length_sec/2.f;
    }
    else{
        // usual step
        step_sec = length_sec/(nb_lines_sec-1);
    }

    width_line_prim = abs(length_prim/(float)nb_lines_sec) + tile_size;
    Point central_point = Point();
    for (size_t i = 0; i < nb_lines_sec; i++)
    {
        if(horizontal){
            if(nb_lines_sec == 1){
                central_point.y = start.y + direction.y*step_sec;
            }
            else{
                central_point.y = start.y + direction.y*i*step_sec;
            }
            central_point.x = start.x + direction.x*(length_prim/(2.f*nb_lines_sec)+i*length_prim/nb_lines_sec);
        }
        else{
            if(nb_lines_sec == 1){
                central_point.x = start.x + direction.x*step_sec;
            }
            else{
                central_point.x = start.x + direction.x*i*step_sec;
            }
            central_point.y = start.y + direction.y*(length_prim/(2.f*nb_lines_sec)+i*length_prim/nb_lines_sec);
        }
        Point new_center = nearestTile(central_point,MINIMUM_TILE_SIZE);
        new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
        Rectangle new_rect;
        if(horizontal) new_rect = calculateRectangle(new_center,width_line_prim,tile_size,(float)screen_width,screen_ratio);
        else new_rect = calculateRectangle(new_center,tile_size,width_line_prim,(float)screen_width,screen_ratio);
        line_rects.insert(line_rects.end(),new_rect);
        
    }
    return line_rects;
}
Rectangle last_rect;
Point last_point;
bool erase_mod = false;
bool last_clicked = false;

void processInput(GLFWwindow* window){

    if(mouseState==GLFW_PRESS){
            // Draw
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            Point point = Point((float)x,(float)y);
            if(point.x >=0 && point.x <= screen_width && point.y>=0 && point.y <= screen_height){

                point = nearestTile(point,MINIMUM_TILE_SIZE);
                Point pointNorm = normalizePosition(point,(float)screen_width,(float)screen_width/screen_ratio);

                if(last_clicked && (abs(last_point.x - point.x)>1.5*MINIMUM_TILE_SIZE || abs(last_point.y - point.y)>1.5*MINIMUM_TILE_SIZE)){
                    //1.5 for floating point errors: points should always be separated by multiples of tile_size
                    std::vector<Rectangle> line_rects = findLine(windowVertices,last_point,point,tile_size);
                    
                    Rectangle fused_rect;
                    for (size_t i = 0; i < line_rects.size(); i++)
                    {
                        if(!erase_mod){
                            if(i==0){
                                fused_rect = line_rects[i].fuseRects(last_rect);
                                if(!fused_rect.isNull()){ 
                                    windowVertices.eraseOneRectangle(last_rect);
                                    drawRect(windowVertices,fused_rect,MAIN_SC_VBO,MAIN_SC_EBO);
                                    last_rect = fused_rect;
                                    
                                }
                                else{
                                    drawRect(windowVertices,line_rects[i],MAIN_SC_VBO,MAIN_SC_EBO);
                                }
                            }
                            else{
                                drawRect(windowVertices,line_rects[i],MAIN_SC_VBO,MAIN_SC_EBO);
                            }
                        }
                        else{
                            bool erased = windowVertices.eraseRectangles(line_rects[i]);
                            if(erased){
                                nbRects=windowVertices.rects.size();
                                drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
                            }
                        }
                    }
                    if(!erase_mod && !line_rects.empty()){
                        if(line_rects.size()==1){
                            last_rect = fused_rect;
                        }   
                        else{
                            last_rect = line_rects[line_rects.size()-1];
                        }
                    }
                    
                }
                else{
                    if(!erase_mod){
                        Rectangle new_rect;
                        new_rect = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
                        if(!last_clicked){
                            drawRect(windowVertices,new_rect,MAIN_SC_VBO, MAIN_SC_EBO);
                        }
                        else{
                            Rectangle fused_rect = new_rect.fuseRects(last_rect);
                            if(fused_rect.isNull()){
                                drawRect(windowVertices,new_rect,MAIN_SC_VBO, MAIN_SC_EBO);
                            }
                            else if(fused_rect != last_rect){
                                new_rect = fused_rect;
                                windowVertices.eraseOneRectangle(last_rect);
                                drawRect(windowVertices,new_rect,MAIN_SC_VBO, MAIN_SC_EBO);
                            }
                            else{
                                new_rect = fused_rect;
                            }
                        }
                        last_rect = new_rect;
                    }
                    else{
                        Rectangle eraser = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
                        bool erased = windowVertices.eraseRectangles(eraser);
                        if(erased){
                            nbRects=windowVertices.rects.size();
                        }
                        drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
                    }
                }
                last_point = point;
            }
        last_clicked = true;
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
        
        if(tile_size+MINIMUM_TILE_SIZE<=MAXIMUM_TILE_SIZE){
            tile_size+=MINIMUM_TILE_SIZE;
            printf("Grow size\n");
        }
    }
    else if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS){
        
        if(tile_size-MINIMUM_TILE_SIZE>=MINIMUM_TILE_SIZE){
            tile_size-=MINIMUM_TILE_SIZE;
            printf("Shrink size\n");
        }
    }
    else if(key == GLFW_KEY_KP_0 && action == GLFW_PRESS){
        
        toogleDebugMode();
        bool debug_active = isDebugModeActive();
        if(debug_active){
            printf("Debug mode activated\n");
        }
        else{
            printf("Debug mode disabled\n");
        }
        drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
        
    }
    else if(key == GLFW_KEY_KP_1 && action == GLFW_PRESS){
        printf("Color red\n");
        active_color={1,0,0};
    }
    else if(key == GLFW_KEY_KP_2 && action == GLFW_PRESS){
        printf("Color green\n");
        active_color={0,1,0};
    }
    else if(key == GLFW_KEY_KP_3 && action == GLFW_PRESS){
        printf("Color blue\n");
        active_color={0,0,1};
    }
    else if(key == GLFW_KEY_KP_9 && action == GLFW_PRESS){
        printf("Color white\n");
        active_color={1,1,1};
    }
    else if(key == GLFW_KEY_KP_8 && action == GLFW_PRESS){
        printf("Color black\n");
        active_color={0,0,0};
    }
    else if(key == GLFW_KEY_KP_ENTER && action == GLFW_PRESS){
        std::string colors_str[] = {"","",""};
        float colors[] = {-1,-1,-1};
        std::cout << "Enter your new color components (<red> <green> <blue> , all between 0 and 1, separated by spaces or line breaks)\n";
        std::cin>>colors_str[0]>>colors_str[1]>>colors_str[2];
        std::cout <<"result: "<< colors[0]<<" "<< colors[1]<<" "<< colors[2]<<" "<<"\n";
        std::regex float_regex = std::regex("0(.[0-9]*|"")|1");
        bool valid_inputs = true;

        for (size_t i = 0; i < 3; i++)
        {
            if(std::regex_match(colors_str[i],float_regex)){
                colors[i] = std::stof(colors_str[i]);
            }
            else{
                std::cout << "The input value nb "<<i<<" is incorrect\n";
                valid_inputs = false;
            }
        }

        if(valid_inputs){
            active_color = {colors[0],colors[1],colors[2]};
            std::cout << "New color: {"<<colors[0]<<", "<<colors[1]<<", "<<colors[2]<<"}\n";
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
    //std::cout << std::fixed << std::setprecision(8); // To print more digits to float, for debugging purposes
    
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
    glGenBuffers(1,&UI_VBO);
    glGenBuffers(1,&UI_EBO);
    
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


    unsigned int UI_VAO;
    glGenVertexArrays(1,&UI_VAO);
    glBindVertexArray(UI_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, UI_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,UI_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,0,0,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, UI_VBO);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2* sizeof(float)));
    glEnableVertexAttribArray(3);

    borderIndicator = Rectangle(Point(-0.5,0.5),0.5,{2,2,2});
    std::vector<float> borderData = borderIndicator.list();
    std::vector<float> borderIndices = {0,1,2,1,2,3};
    glBindBuffer(GL_ARRAY_BUFFER, UI_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*borderData.size(), &borderData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,UI_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*borderIndices.size(),&borderIndices[0],GL_STATIC_DRAW);
    

    while(!glfwWindowShouldClose(app_window)){
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        mouseState = glfwGetMouseButton(app_window, GLFW_MOUSE_BUTTON_LEFT);
        processInput(app_window);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,nbRects*8,GL_UNSIGNED_INT,0); //number 2: total number of vertices

        glBindVertexArray(UI_VAO);
        glDrawElements(GL_TRIANGLES,8,GL_UNSIGNED_INT,0); //number 2: total number of vertices
        
        glBindVertexArray(0);

        // Necessary for any window
        glfwPollEvents();
        glfwSwapBuffers(app_window);
    }
    glfwTerminate();
    return 0;
}