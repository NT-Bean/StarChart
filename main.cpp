// failed build count: 10

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions);
std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions);

struct Star
{
    std::string name = "unknown star";
    float radius = 6975000.0f; // in kilometers -- i'll probably convert it over once all is said and done
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is whatever unit i end up using

    std::vector<Vertex> vertices = {};
    std::vector<GLuint> indices = {};
    VAO starVAO;

    std::vector<Vertex> sunflareVertices = {};
    std::vector<Vertex> sunflareIndices = {};
    VAO sunflareVAO;

    Star(std::string name, float radius, glm::vec3 color, glm::vec3 position, int subdivisions)
    {
        if (subdivisions % 4 != 0 || subdivisions < 12)
        {
            subdivisions = 16;
        }

        this->name = name;
        this->radius = radius;
        this->color = color;
        this->position = position;

        std::cout << "New star generated named " << name << std::endl;
        std::cout << name << "radius (R_sol) is: " << radius << std::endl;
        vertices = defineSphereVertices(radius, color, position, subdivisions);
        std::cout << name << "'s vertices defined. length of vertices is " << vertices.size() << std::endl;

        indices = defineSphereIndices(vertices, subdivisions);
        std::cout << name << "'s indices defined. length of indices is " << indices.size() << std::endl;

        starVAO.Bind();
        VBO VBO(vertices);
        EBO EBO(indices);

        starVAO.LinkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
        starVAO.LinkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

        starVAO.Unbind();
        VBO.Unbind();
        EBO.Unbind();
        std::cout << name << "'s VAO successfully generated and configured" << std::endl << std::endl;


    }

    void Draw(Shader& shader, Camera& camera)
    {
        shader.Activate();
        starVAO.Bind();

        glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(45.0f, 0.1f, 1e6f, shader, "camMatrix");

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
         Star("sol",              1.0f,     glm::vec3(1.0f,  1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f),    128),
         Star("proxima centauri", 0.1542f,  glm::vec3(0.75f, 0.1f, 0.1f), glm::vec3(200.0f, 0.0f, 0.0f), 128)
    };


    /*Star renderedStar = proximaCentauri;

    VAO VAO1;
    VAO1.Bind();

    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO1(renderedStar.vertices);
    // Generates Element Buffer Object and links it to indices
    EBO EBO1(renderedStar.indices);

    // Links VBO attributes such as coordinates and colors to VAO
    VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    VAO1.LinkAttribute(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));
    // Unbind all to prevent accidentally modifying them
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind(); */


    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

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
        //camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");


        //VAO1.Bind();
        //glDrawElements(GL_TRIANGLES, renderedStar.indices.size(), GL_UNSIGNED_INT, 0);

        for (int i = 0; i < std::size(stars); i++)
        {
            stars[i].Draw(shaderProgram, camera);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();*/
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
    std::cout << "The number of requested subdivisions is " << subdivisions << std::endl;
    for (int i = 0; i < subdivisions; i++) // [0...15]
    {
        float heading = ((float)i * 2.0f * pi) / (float)subdivisions;
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