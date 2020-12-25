// GL_SNAKE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <sstream>
#include <fstream>
#include "Snake.h"
#include "Treat.h"



struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource parseShader(const std::string& filepath)
{
    enum ShaderType
    {
        vertex,
        fragment,
        NONE
    };

    std::fstream stream;
    stream.open(filepath);
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    ss[0].str("");
    ss[1].str("");
    if (stream.is_open())
    {
        std::string line;
        while (getline(stream, line))
        {
            //std::cout << line << "\n";
            if (line.find("shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = ShaderType::vertex;
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    type = ShaderType::fragment;
                }
            }
            else
            {
                //string stream array contains both shaders
                ss[(int)type] << line << "\n";
            }
        }
    }
    stream.close();

    ShaderProgramSource source{ ss[0].str(), ss[1].str() };
    return source;
}


//GLenum is of type unsigned int, we specify the type of shader
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    //id is the int handle to the shader
    unsigned int id = glCreateShader(type);

    const char* src = source.c_str();

    //shadersource sets the source code of the shader to string src.
    //specifying nullptr means null terminated string.
    glShaderSource(id, 1, &src, nullptr);

    glCompileShader(id);

    //error handling code (compilation failures)
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];

        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile shader" << std::endl;
        std::cout << message << std::endl;

        delete[] message;
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //shaders must be detached before they are deleted
    //gldetachshader

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


void processInput(GLFWwindow* window, Snake* s)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        if (s->CurrentDirection == Down)
            s->CurrentDirection = Down;
        else
            s->CurrentDirection = Up;
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        if (s->CurrentDirection == Up)
            s->CurrentDirection = Up;
        else
            s->CurrentDirection = Down;
 

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        if (s->CurrentDirection == Right)
            s->CurrentDirection = Right;
        else
        s->CurrentDirection = Left;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        if (s->CurrentDirection == Left)
            s->CurrentDirection = Left;
        else
            s->CurrentDirection = Right;

}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        return -1;
    }

    float points[] =
    {
        -0.5f, -.0f, 0.5f,
        0.5f, -.0f, 0.5f,
        0.5f, .0f, -0.5f,
        -0.5, .0f, -0.5f
    };

    unsigned int indices[] =
    {
        0,1,
        1,2,
        2,3,
        3,0
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);




    ShaderProgramSource source = parseShader("C:\\Users\\Zachary Alonzo\\source\\repos\\GL_SNAKE\\GL_SNAKE\\Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

   
    glm::mat4 project = glm::mat4(1.0);
    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 view = glm::mat4(1.0);
    glm::mat4 mvp = glm::mat4(1.0);
    
    unsigned int mvpLoc = glGetUniformLocation(shader, "mvp");

    view = glm::mat4(1.0);
    view = glm::translate(view, glm::vec3(0, 0, -20));
    view = glm::rotate(view, glm::radians(30.f), glm::vec3(1, 0, 0));

    project = glm::perspective(glm::radians(45.f), 640.f / 480.f, 0.1f, 100.f);


    glfwSwapInterval(7);
    Snake* snake = new Snake();
    Treat* treat = new Treat();

    snake->treat = treat;
   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        //get snake input, update directions
        processInput(window, snake);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        model = glm::mat4(1.0);       
        model = glm::translate(model, glm::vec3(0 ,0, 0));
        //model = glm::rotate(model, glm::radians(-70.f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(10, 0, 10));
        //glm::mat4 view = glm::mat4(1.0);
        
        mvp = project * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);      
        
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);

        
        snake->Draw(view, project);
        snake->UpdateSnake();
        //snake.UpdateSnake();

        glUseProgram(shader);
        model = glm::mat4(1.0);
        model = glm::translate(model, treat->pos);
        mvp = project * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        treat->Draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}