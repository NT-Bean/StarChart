// failed build count: 47

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

#include "starsystem.h"
#include "systems.h"

typedef StarSystem::Star Star;
typedef StarSystem::AstroCoords AstroCoords;


int width = 1280;
int height = 720;

// consts
constexpr float pi = glm::pi<float>();

std::vector<Vertex> equatorialPlane =
{
    Vertex(glm::vec3(-500.0f, 0.0f, -500.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(500.0f, 0.0f, -500.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(-500.0f, 0.0f, 500.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),

    Vertex(glm::vec3(500.0f, 0.0f, -500.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(-500.0f, 0.0f, 500.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(500.0f, 0.0f, 500.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            std::cout << "i need help too man, the spheres aren't rendering" << std::endl << std::endl;
            std::cout << "'-f': prevents flares from rendering" << std::endl;
            std::cout << "'-h': take a wild guess" << std::endl << std::endl;
            return 0;
        }
        if (strcmp(argv[i], "-f") == 0)
        {
            StarSystem::renderFlares = false;
            std::cout << "flares disabled" << std::endl;
        }
    }

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


    StarSystem::defaultStarShader = Shader("resources/star.vert", "resources/star.frag");
    StarSystem::defaultFlareShader = Shader("resources/flare.vert", "resources/flare.frag");
    StarSystem::defaultFlareTex = Texture("resources/flare.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);


    Systems::init(
        std::vector<StarSystem> {

        StarSystem(std::vector<Star> {
            Star("Sol", 1.0f, 5772.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), 128)
        }, "Solar System", AstroCoords(0, 0, 0.0f, 0, 0, 0.0f, 0.0f), 0.1f),

        StarSystem(std::vector<Star> {
            Star("Sol", 1.0f, 5772.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), 128)
        }, "Alpha Centauri", AstroCoords(0, 0, 0.0f, 0, 0, 0.0f, 0.0f), 0.1f),
    });

    /*
    StarSystem(std::vector<Star> {
                Star("Procyon A", 2.043f, 6582.0f, 7.049f, glm::vec3(0.0f,0.0f,0.0f), 128)
            }, "Procyon", AstroCoords(7, 39, 18.11950, 5, 13, 29.9552, 11.46f), 0.1f)    
    */
    std::cout << "ardess " << Systems::systems[1].bodies[0].system << std::endl;
    glm::vec3 whadafuq = Systems::systems[1].bodies[0].position + Systems::systems[1].bodies[0].system->position;
    std::cout << " absolute pos of pork yawn is (" << whadafuq.x << ", " << whadafuq.y << ", " << whadafuq.z << ")" << std::endl;

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 1.58125e-5f) * StarSystem::scale, StarSystem::scale);

    /* // uncomment for the equatorial plane
    VAO planeVAO;
    planeVAO.Bind();
    VBO planeVBO(equatorialPlane);
    planeVAO.LinkAttribute(planeVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    planeVAO.LinkAttribute(planeVBO, 1, 4, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    planeVAO.Unbind();
    planeVBO.Unbind();
    */

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
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

        if (Systems::boundSystem != -1)
        {
            float camDistance = glm::distance(Systems::systems[Systems::boundSystem].position, camera.Position / StarSystem::scale);
            camera.speed = (camDistance <= 4e-6) ? (5e-5f) : (glm::min(5e5f * camDistance * camDistance + 0.005f, 200.0f));
            std::cout << "speed: " << camera.speed << " dist: " << camDistance << std::endl;
        }

        camera.Inputs(window, StarSystem::scale);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetFramebufferSize(window, &camera.width, &camera.height);

        Systems::checkInfluence(camera);

        /* // uncomment to enable drawing the equatorial plane
        shaderProgram.Activate();
        planeVAO.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        planeVAO.Unbind();
        */ // this all needs to be retrofitted

        Systems::drawAll(camera);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Systems::deleteAll();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}