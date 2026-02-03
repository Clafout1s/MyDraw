#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <math.h>
#include <cmath>
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
const float MINIMUM_TILE_SIZE = 5;

vertexData windowVertices;
unsigned int MAIN_SC_VBO;
unsigned int MAIN_SC_EBO;

Rectangle borderIndicator;
unsigned int UI_VBO;
unsigned int UI_EBO;

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
    Rectangle square = Rectangle(point,width,height);
    return square;
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
/**
 * We consider that start and end are in pixel coordinates.
 */
std::vector<Rectangle> findLine(vertexData& data, Point& start, Point& end, float tile_size){

    //std::cout << "Begin line\n";
    std::vector<Rectangle> line_rects = {};
    float length_x = abs(end.x - start.x);
    float length_y = abs(end.y - start.y);
    Point direction = Point(sign(end.x-start.x),sign(end.y-start.y));
    
    if(abs(length_x) >= abs(length_y)){
        // Horizontal majority in the line
        int nb_lines_y = 2* floor((length_y+tile_size)/(float)tile_size)-1;
        float step_y;
        if(nb_lines_y<=1 && nb_lines_y != 0){
            nb_lines_y = 2;
        }
        if(nb_lines_y==0){
            nb_lines_y=1;
            step_y = length_y/2.f;
        } 
        else{
            step_y = length_y/(nb_lines_y-1);
        }
    
        float width_line_x = abs(length_x / (float)nb_lines_y + tile_size);
        
        Point central_point = Point();
        //std::cout << "tile size: "<<tile_size<<"; nb lines: "<< nb_lines_y << "; step: " << step_y << "; length x: " <<length_x<< "; length y: " <<length_y<<"\n";
        for (size_t i = 0; i < nb_lines_y; i++)
        {
            if(nb_lines_y == 1){
                central_point.y = start.y + direction.y*step_y;
            }
            else{
                central_point.y = start.y + direction.y*i*step_y;
            }
            central_point.x = start.x + direction.x*(length_x/(2.f*nb_lines_y)+i*length_x/(nb_lines_y));

            Point new_center = nearestTile(central_point,MINIMUM_TILE_SIZE);
            new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
            Rectangle new_rect = calculateRectangle(new_center,width_line_x,tile_size,(float)screen_width,screen_ratio);
            line_rects.insert(line_rects.end(),new_rect);
        }
    }
    else{
        // Vertical majority in the line

        int nb_lines_x = 2* floor((length_x+tile_size)/(float)tile_size)-1;
        float step_x;
        if(nb_lines_x<=1 && nb_lines_x != 0){
            nb_lines_x = 2;
        }
        if(nb_lines_x==0){
            nb_lines_x=1;
            step_x = length_x/2.f;
        } 
        else{
            step_x = length_x/(nb_lines_x-1);
        }
    
        float width_line_y = abs(length_y / (float)nb_lines_x + tile_size);
        
        Point central_point = Point();
        //std::cout << "tile size: "<<tile_size<<"; nb lines: "<< nb_lines_y << "; step: " << step_y << "; length x: " <<length_x<< "; length y: " <<length_y<<"\n";
        for (size_t i = 0; i < nb_lines_x; i++)
        {
            if(nb_lines_x == 1){
                central_point.x = start.x + direction.x*step_x;
            }
            else{
                central_point.x = start.x + direction.x*i*step_x;
            }
            central_point.y = start.y + direction.y*(length_y/(2.f*nb_lines_x)+i*length_y/(nb_lines_x));

            Point new_center = nearestTile(central_point,MINIMUM_TILE_SIZE);
            new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
            Rectangle new_rect = calculateRectangle(new_center,tile_size,width_line_y,(float)screen_width,screen_ratio);
            line_rects.insert(line_rects.end(),new_rect);
        }
    }

    //std::cout << "End line\n\n";
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
                    for (size_t i = 0; i < line_rects.size(); i++)
                    {
                        if(!erase_mod){
                            drawRect(windowVertices,line_rects[i],MAIN_SC_VBO,MAIN_SC_EBO);
                        }
                        else{
                            bool erased = windowVertices.eraseRectangles(line_rects[i]);
                            if(erased){
                                nbRects=windowVertices.rects.size();
                                drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
                            }
                        }
                    }
                }
                else{
                    if(!erase_mod){
                        Rectangle new_rect = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
                        drawRect(windowVertices,new_rect,MAIN_SC_VBO, MAIN_SC_EBO);

                    }
                    else{
                        Rectangle eraser = calculateSquare(pointNorm,tile_size,(float)screen_width,screen_ratio);
                        bool erased = windowVertices.eraseRectangles(eraser);
                        
                        if(erased){
                            printf("%ld\n",windowVertices.indices.size());
                            nbRects=windowVertices.rects.size();
                            drawFigure(windowVertices,MAIN_SC_VBO,MAIN_SC_EBO);
                        }
                    }
            
                }
                last_point = point;
                last_clicked = true;
            }
            else{
                last_clicked=false;
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
        std::cout << windowVertices.rects.size()+3*sizeof(float)+4*3*sizeof(float) << "\n"; 
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

    borderIndicator = Rectangle(Point(-0.5,0.5),0.5);
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

// Old code line for backup: 


        /*
        int n_squares = abs(length_x) / margin_space ;
        float y_reference_line = start.y;
        for (size_t i = 0; i < n_squares; i++)
        {
            float new_point_x = start.x + (float)sign(length_x)*(i*margin_space);
            float new_point_y = a * new_point_x + b;
            Point new_point_nearest = nearestTile(Point(new_point_x,new_point_y),margin_space);

            if(!equalF(new_point_nearest.y,y_reference_line) || i+1==n_squares){
                Point new_rec_center = (nb_squares_line==1)?
                    Point(start.x + tile_size/2,y_reference_line) :
                    Point(start.x + nb_squares_line*(float)sign(length_x)*(margin_space/2.f),y_reference_line);
                float width = (nb_squares_line==1)? tile_size : nb_squares_line*margin_space;

                new_rec_center = normalizePosition(new_rec_center,(float)screen_width,(float)screen_width/screen_ratio);
                Rectangle new_rec = calculateRectangle(new_rec_center,width,tile_size,(float)screen_width,screen_ratio);
                line_rects.insert(line_rects.end(),new_rec);

                nb_squares_line = 1;
                y_reference_line = new_point_nearest.y;

            }
            else{
                nb_squares_line++;
            }
        }
                
            /*
            if(abs(other_axis_value-new_rec_point.y)>margin_space){
                Point new_center = nearestTile(new_rec_point,5);
                new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
                Rectangle new_rect = calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio);
                line_rects.insert(line_rects.end(),new_rect);
                new_rec_length = margin_space;
                other_axis_value = new_rect.center_p.y;
            }
            else{
                new_rec_point.x+=(float)sign(length_x)*(margin_space/2.f);
                new_rec_point.y = a * new_rec_point.x+(margin_space/2.f) + b;
                new_rec_length += margin_space;
            }
            /*
            Point new_center_temp = Point();
            new_center_temp.x = start.x + (float)sign(length_x)*(i*margin_space); 
            new_center_temp.y = a * new_center_temp.x + b;
            Point new_center = nearestTile(new_center_temp,5);
            length_rec+= margin_space;
            if(!equalF(new_center.y,other_axis_value) || (i+1==n_squares)){
                Point rec_center = Point()
                new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
                Rectangle new_rect = calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio);
                line_rects.insert(line_rects.end(),new_rect);
                length_rec = margin_space;
                other_axis_value = new_rect.center_p.y;
            }
            */
            
    /*
    else{
        int n_squares = abs(length_y) / margin_space ;
        other_axis_value = start.x;
        for (size_t i = 1; i < n_squares; i++)
        {
            Point new_center_temp = Point();
            new_center_temp.y = start.y + (float)sign(length_y)*(i*margin_space);
            new_center_temp.x = (new_center_temp.y - b)/a;
            Point new_center = nearestTile(new_center_temp,5);
            new_center = normalizePosition(new_center,(float)screen_width,(float)screen_width/screen_ratio);
            line_rects.insert(line_rects.end(),calculateSquare(new_center,tile_size,(float)screen_width,screen_ratio));
        }
    }*/