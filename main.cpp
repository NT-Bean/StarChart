// failed build count: 4

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "Camera.h"

int width = 900;
int height = 900;

std::vector<Vertex> vertices =
{
    Vertex( glm::vec3( 0.0f,  0.0f,  0.0f)), // origin
    Vertex( glm::vec3( 0.5f,  0.0f,  0.0f)), // forward i think
    Vertex( glm::vec3( 0.0f,  0.5f,  0.0f)), // up
    Vertex( glm::vec3( 0.0f,  0.0f,  0.5f))  // right i think
};

std::vector<GLuint> indices =
{
    0, 1, 2,
    0, 3, 2,
    0, 3, 1
};


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "StarChart", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "woops the window didnt get make properly" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    gladLoadGL();
    glViewport(0, 0, width, height);


    Shader shaderProgram("default.vert", "default.frag");


    VAO VAO1;
    VAO1.Bind();

    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO1(vertices);
    // Generates Element Buffer Object and links it to indices
    EBO EBO1(indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    // Unbind all to prevent accidentally modifying them
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    glEnable(GL_DEPTH_TEST);

    glfwSwapInterval(1); // turn on vsync (limit framrate)

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        camera.Inputs(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}