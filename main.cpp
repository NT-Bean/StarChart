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
    Vertex(glm::vec3(-10.0f, 0.0f, -5000.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(10.0f, 0.0f, -5000.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),

    Vertex(glm::vec3(-10.0f, 0.0f, -5000.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(10.0f, 0.0f, -5000.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)),
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
            //   name   rad.  temp. lumin.      astronomical coordinates    dist.  # of subdivisions
            Star("Sol", 1.0f, 5772, 1.0f, AstroCoords(0, 0, 0.0, 0, 0, 0.0, 0.0f), 128)
        }, "Solar System", AstroCoords(0, 0, 0.0, 0, 0, 0.0, 0.0f), 0.1f),

        StarSystem(std::vector<Star> {
            Star("Alpha Centauri A", 1.2175f, 5804, 1.5059f, AstroCoords(14, 39, 36.494, -60, 50, 2.3737, 4.344f), 128),
            Star("Alpha Centauri B", 0.8591f, 5207, 0.4981f, AstroCoords(14, 39, 36.06311, -60, 50, 15.0992, 4.344f), 128),
            Star("Proxima Centauri", 0.1542f, 2992, 0.00005f, AstroCoords(14, 29, 42.946, -62, 40, 46.16, 4.2465f), 96)
        }, "Alpha Centauri", AstroCoords(14, 39, 36.27856, -60, 50, 8.73645, 4.34399999722f), 0.3f),

        StarSystem(std::vector<Star> {
            Star("Barnard's Star", 0.187f, 3195, 0.0004f, AstroCoords(17, 57, 48.49847, 4, 41, 36.1139, 5.9629f), 96)
        }, "Barnard's Star (sys)", AstroCoords(17, 57, 48.49847, 4, 41, 36.1139, 5.9629f), 0.05f),

        StarSystem(std::vector<Star> {
            Star("Sirius A", 1.7144f, 9845, 24.74f, AstroCoords(6, 45, 08.917, -16, 42, 58.02, 8.61f), 128),
            Star("Sirius B", 0.008098f, 25000, 0.0f, AstroCoords(6, 45, 9.0, -16, 43, 6.0, 8.61f), 64)
        }, "Sirius", AstroCoords(6, 45, 08.917, -16, 42, 58.02, 8.61f), 0.1f)
    });

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 1.58125e-5f) * StarSystem::scale, StarSystem::scale);

    // uncomment for the equatorial plane
    VAO planeVAO;
    planeVAO.Bind();
    VBO planeVBO(equatorialPlane);
    planeVAO.LinkAttribute(planeVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    planeVAO.LinkAttribute(planeVBO, 1, 4, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    planeVAO.Unbind();
    planeVBO.Unbind();
    

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);

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

        if (Systems::boundSystem != -1)
        {
            float camDistance = glm::distance(Systems::systems[Systems::boundSystem].position, camera.Position / StarSystem::scale);
            camera.speed = (camDistance <= 4e-6) ? (5e-9f * StarSystem::scale) : (glm::min(5e1f * StarSystem::scale * camDistance * camDistance + 5e-7f * StarSystem::scale, 0.01f * StarSystem::scale));
            // std::cout << "speed: " << camera.speed << " dist: " << camDistance << std::endl;
        }
        else
        {
            camera.speed = 200.0f;
        }


        camera.Inputs(window, StarSystem::scale);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetFramebufferSize(window, &camera.width, &camera.height);

        Systems::checkInfluence(camera);

        // uncomment to enable drawing the equatorial plane
        StarSystem::defaultStarShader.Activate();
        planeVAO.Bind();
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        planeVAO.Unbind();

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