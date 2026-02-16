// failed build count: 11

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
#include "camera.h"

int width = 1280;
int height = 720;

// consts
float pi = glm::pi<float>();
float L0 = 78.7248f; // zero-point luminosity; defined such that the Sun's M_bol = 4.74


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions);
std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions);

struct Star
{
    std::string name = "unknown star";
    float radius = 1.0f; // in solar radii
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is whatever unit i end up using

    std::vector<Vertex> vertices = {};
    std::vector<GLuint> indices = {};
    VAO starVAO;

    float luminosity;

    std::vector<Vertex> sunflareVertices = {};
    std::vector<GLuint> sunflareIndices = {};
    VAO sunflareVAO;

    Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 position, int subdivisions)
    {
        if (subdivisions % 4 != 0 || subdivisions < 12)
        {
            subdivisions = 16;
        }

        this->name = name;
        this->radius = radius;
        this->color = color;
        this->luminosity = luminosity;
        this->position = glm::vec3(position.x, position.y, position.z);

        std::cout << "New star generated named " << name << std::endl;
        std::cout << name << "radius (R_sol) is: " << radius << std::endl;
        vertices = defineSphereVertices(radius, color, position, subdivisions);
        std::cout << name << "'s vertices defined. length of vertices is " << vertices.size() << std::endl;

        indices = defineSphereIndices(vertices, subdivisions);
        std::cout << name << "'s indices defined. length of indices is " << indices.size() << std::endl;

        starVAO.Bind();
        VBO starVBO(vertices);
        EBO starEBO(indices);

        starVAO.LinkAttribute(starVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
        starVAO.LinkAttribute(starVBO, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

        starVAO.Unbind();
        starVBO.Unbind();
        starEBO.Unbind();


        sunflareVertices = defineSphereVertices(radius * 2, glm::vec3(1.0f, 0.0f, 1.0f), position, 16);
        sunflareIndices = defineSphereIndices(sunflareVertices, 16);

        sunflareVAO.Bind();
        VBO sunflareVBO(sunflareVertices);
        EBO sunflareEBO(sunflareIndices);


        sunflareVAO.LinkAttribute(sunflareVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
        sunflareVAO.LinkAttribute(sunflareVBO, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

        sunflareVAO.Unbind();
        sunflareVBO.Unbind();
        sunflareEBO.Unbind();

        std::cout << name << "'s VAOs successfully generated and configured" << std::endl << std::endl;
    }

    void Draw(Shader& shader, Camera& camera)
    {
        shader.Activate();

        starVAO.Bind();

        glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(45.0f, 0.1f, 1e9f, shader, "camMatrix");

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        starVAO.Unbind();

        float apparentMagnitude = 0.0f;
        sunflareVertices = defineSphereVertices(radius * 0.05 * glm::distance(camera.Position, position), glm::vec3(0.0f, 0.0f, 1.0f), position, 16);
        sunflareIndices = defineSphereIndices(sunflareVertices, 16);

        sunflareVAO.Bind();
        VBO sunflareVBO(sunflareVertices);
        EBO sunflareEBO(sunflareIndices);


        sunflareVAO.LinkAttribute(sunflareVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
        sunflareVAO.LinkAttribute(sunflareVBO, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

        glDrawElements(GL_TRIANGLES, sunflareIndices.size(), GL_UNSIGNED_INT, 0);
        sunflareVAO.Unbind();
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

    Star stars[2] =
    {
         Star("sol",       1.0f,   glm::vec3(1.0f, 1.0f, 1.0f), 1.0f,   glm::vec3(0.0f, 0.0f, 0.0f),   128),
         Star("procyon a", 2.043f, glm::vec3(0.9f, 0.9f, 1.0f), 7.049f, glm::vec3(11.46f, 0.0f, 0.0f), 128)
    };


    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

    glEnable(GL_DEPTH_TEST);

    glfwSwapInterval(1); // turn on vsync (limit framrate)


    double prevTime = glfwGetTime();
    std::cout << "FPS: ";

    while(!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        int FPS = 1 / (currentTime - prevTime);
        prevTime = currentTime;
        std::cout << "\x1b[2K\rFPS: " << FPS;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        camera.Inputs(window);
        // Updates and exports the camera matrix to the Vertex Shader
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetFramebufferSize(window, &camera.width, &camera.height);


        for (int i = 0; i < std::size(stars); i++)
        {
            stars[i].Draw(shaderProgram, camera);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i = 0; i < std::size(stars); i++)
    {
        stars[i].starVAO.Delete();
        stars[i].sunflareVAO.Delete();
    }
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}




std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions)
{
    std::vector<Vertex> vertices = {};
    double radiusLy = radius * 0.735355f; // converts solar radius to 1/10,000,000 of a light year. sure, arbitrary, i know. i dont care
    std::cout << "generating points. radiusLy is defined as " << radiusLy << std::endl;
    for (int i = 0; i < subdivisions; i++) // [0...15]
    {
        float heading = ((float)i * 2.0f * pi) / (float)subdivisions;
        for (int j = 0; j <= subdivisions / 2; j++) // [0...d/2]
        {
            float azimuth = ((float)j * 2.0f * pi) / (float)subdivisions + (pi / 2);
            Vertex currentVertex =
                Vertex(glm::vec3(
                    radiusLy * glm::cos(heading) * glm::cos(azimuth) + position.x * 10000000.0f,
                    radiusLy * glm::sin(azimuth) + position.y * 10000000.0f,
                    radiusLy * glm::sin(heading) * glm::cos(azimuth) + position.z * 10000000.0f
                    
                ), color);
            vertices.push_back(currentVertex);
        }
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