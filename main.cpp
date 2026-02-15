// failed build count: 7

#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "Camera.h"

int width = 1600;
int height = 900;

// consts
float pi = glm::pi<float>();

std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions);
std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions);


//std::vector<Vertex> vertices =
//{
//    Vertex( glm::vec3( 1.0f,  0.0f,  1.0f), glm::vec3( 1.0f, 0.0f, 1.0f) ),
//    Vertex( glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3( 1.0f, 0.0f, 0.0f) ), 
//    Vertex( glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.0f, 0.0f, 1.0f) ), 
//    Vertex( glm::vec3( 0.0f,  0.0f,  0.0f), glm::vec3( 0.0f, 1.0f, 0.0f) )  
//};
//
//std::vector<GLuint> indices =
//{
//    0, 1, 2,
//    1, 2, 3,
//};


struct Star
{
    std::string name = "unknown star";
    float radius = 6975000.0f; // in kilometers -- i'll probably convert it over once all is said and done
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is whatever unit i end up using

    std::vector<Vertex> vertices = {};
    std::vector<GLuint> indices = {};

    Star(std::string name, float radius, glm::vec3 color, glm::vec3 position, int subdivisions)
    {
        if (subdivisions % 4 != 0)
        {
            subdivisions = 16;
        }
        subdivisions = glm::max<int>(subdivisions, 12);

        this->name = name;
        this->radius = radius;
        this->color = color;
        this->position = position;

        std::cout << "New star generated named " << name << std::endl;
        std::cout << "Radius (km) is: " << radius << std::endl;
        vertices = defineSphereVertices(radius, color, position, subdivisions);
        std::cout << "Sphere vertices defined. The length of `vertices` of star " << name << " is " << vertices.size() << std::endl;

        indices = defineSphereIndices(vertices, subdivisions);
        std::cout << "Sphere indices defined. The length of `indices` of star " << name << " is " << indices.size() << std::endl;
    }

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



    Shader shaderProgram("default.vert", "default.frag");


    Star sun("sol", 6975000.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 10.0f, -3.0f), 16);

    VAO VAO1;
    VAO1.Bind();

    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO1(sun.vertices);
    // Generates Element Buffer Object and links it to indices
    EBO EBO1(sun.indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    VAO1.LinkAttribute(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));
    // Unbind all to prevent accidentally modifying them
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


    Camera camera(width, height, glm::vec3(0.0f, 0.5f, 2.0f));

    glEnable(GL_DEPTH_TEST);

    glfwSwapInterval(1); // turn on vsync (limit framrate)

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        camera.Inputs(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
        glViewport(0, 0, width, height);

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sun.indices.size(), GL_UNSIGNED_INT, 0);


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

std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions)
{
    std::vector<Vertex> vertices = {};
    double radiusLy = radius * 1.0570008340247E-7; // converts km to 1/1,000,000 of a light year. sure, arbitrary, i know. i dont care
    std::cout << "generating points. radiusLy is defined as " << radiusLy << std::endl;
    std::cout << "The number of requested subdivisions is " << subdivisions << std::endl;
    for (int i = 0; i < subdivisions; i++) // [0...15]
    {
        float heading = ((float)i * 2.0f * pi) / (float)subdivisions;
        std::cout << "heading: " << heading << std::endl;
        for (int j = 0; j <= subdivisions / 2; j++) // [0...d/2]
        {
            float azimuth = ((float)j * 2.0f * pi) / (float)subdivisions + (pi / 2);
            Vertex currentVertex =
                Vertex(glm::vec3(
                    radiusLy * glm::cos(heading) * glm::cos(azimuth) + position.x,
                    radiusLy * glm::sin(azimuth) + position.y,
                    radiusLy * glm::sin(heading) * glm::cos(azimuth) + position.z
                    
                ), color);
            vertices.push_back(currentVertex);

            std::cout << "azimuth: " << azimuth << std::endl;
        }
        std::cout << std::endl;
    }

    return vertices;
}

std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions)
{
    std::vector<GLuint> indices = {};
    int layerCount = subdivisions / 2 + 1;
    for (int i = 1; i <= subdivisions - ((subdivisions - 1) % 2); i += 2) // horizontal
    {
        for (int j = 1; j <= layerCount - 2; j += 2) // vertical
        {
            int index = (j + layerCount * i);
            int overflowCheckedIndex = (index + layerCount > subdivisions * layerCount) ? (index + layerCount - subdivisions * layerCount) : (index + layerCount);
            int underflowCheckedIndex = (index - layerCount < 1) ? (index - layerCount + subdivisions * layerCount) : (index - layerCount);

            // top right
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(overflowCheckedIndex);

            // bottom left
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(underflowCheckedIndex);

            // top left
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(underflowCheckedIndex);

            // bottom right
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(overflowCheckedIndex);
        }
    }

    for (int i = 0; i <= (subdivisions - 2) - (subdivisions - 4) % 2; i += 2) // horizontal
    {
        for (int j = 2; j <= layerCount - 3; j += 2) // vertical
        {
            int index = (j + layerCount * i);
            int overflowCheckedIndex = (index + layerCount > subdivisions * layerCount) ? (index + layerCount - subdivisions * layerCount) : (index + layerCount);
            int underflowCheckedIndex = (index - layerCount < 1) ? (index - layerCount + subdivisions * layerCount) : (index - layerCount);

            // top right
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(overflowCheckedIndex);

            // bottom left
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(underflowCheckedIndex);

            // top left
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(underflowCheckedIndex);

            // bottom right
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(overflowCheckedIndex);

            // std::cout << index << std::endl;
        }
    }

    return indices;
}