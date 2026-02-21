// failed build count: 27

#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "camera.h"
#include "texture.h"

#include "stars.h"

int width = 1280;
int height = 720;

// consts
constexpr float pi = glm::pi<float>();


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
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

    Shader shaderProgram("resources/star.vert", "resources/star.frag");
    Shader flareShaderProgram("resources/flare.vert", "resources/flare.frag");

    Texture flareTex("resources/flare.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

    Stars stars = Stars(
        std::vector<Stars::Star> {
            Stars::Star("sol",          1.0f,   glm::vec3(1.0f, 1.0f, 1.0f), 1.0f,      glm::vec3( 0.0, 0.0, 0.0),  128),
            Stars::Star("procyon a",    2.043f, glm::vec3(0.9f, 0.9f, 1.0f), 7.049f,    glm::vec3( 1.0, 0.0, 0.0),  128),
            Stars::Star("evil sol",     1.0f,   glm::vec3(1.0f, 0.0f, 0.0f), 1.0f,      glm::vec3(-1.0, 0.0, 0.0),  128)
    }, flareTex, shaderProgram, flareShaderProgram, 0.1f);


    Camera camera(width, height, glm::vec3(0.0, 0.0, 1.58125e-5) * Stars::scale, Stars::scale);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_PROGRAM_POINT_SIZE);

    glfwSwapInterval(1); // turn on vsync (limit framrate)

    double prevTime = glfwGetTime();
    // std::cout << "FPS: ";


    while(!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        int FPS = 1 / (currentTime - prevTime);
        prevTime = currentTime;
        // std::cout << "\x1b[2K\rFPS: " << FPS;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        camera.Inputs(window, Stars::scale);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetFramebufferSize(window, &camera.width, &camera.height);

        stars.checkInfluence(camera);

        stars.drawAll(shaderProgram, flareShaderProgram, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    stars.deleteAll();
    shaderProgram.Delete();
    flareShaderProgram.Delete();
    flareTex.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}