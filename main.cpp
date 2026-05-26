// failed build count: 60

#ifdef _WIN32
    #include <windows.h>
    #define sleepy(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleepy(ms) usleep(ms * 1000) // usleep uses microseconds
#endif

#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

#include "starsystem.h"
#include "systems.h"

#include "text.h"


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

void printMarquee();

int main(int argc, char **argv)
{
    printMarquee();

    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            std::cout << "'-f': prevents flares from rendering" << std::endl;
            std::cout << "'-v': enables verbose logging. VERY verbose." << std::endl;
            std::cout << "'-h': take a wild guess" << std::endl << std::endl;
            return 0;
        }
        if (strcmp(argv[i], "-f") == 0)
        {
            StarSystem::renderFlares = false;
            std::cout << "flares disabled" << std::endl;
        }
        if (strcmp(argv[i], "-v") == 0)
        {
            StarSystem::verboseLog = true;
            std::cout << "verbose logging enabled" << std::endl;
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


    StarSystem::defaultStarShader = Shader("resources/shaders/star.vert", "resources/shaders/star.frag");
    StarSystem::defaultFlareShader = Shader("resources/shaders/flare.vert", "resources/shaders/flare.frag");
    StarSystem::defaultFlareTex = Texture("resources/textures/flare.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

    std::cout << "UI FONT INIT" << std::endl;
    StarSystem::uiFont = Font("resources/fonts/msgothic.ttc", 24);
 
    Shader uiShader = Shader("resources/shaders/font.vert", "resources/shaders/font.frag");

    Systems::init();

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 1.58125e-5f) * StarSystem::scale, StarSystem::scale);

    // uncomment for the equatorial plane
    //VAO planeVAO;
    //planeVAO.Bind();
    //VBO planeVBO(equatorialPlane);
    //planeVAO.LinkAttribute(planeVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    //planeVAO.LinkAttribute(planeVBO, 1, 4, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    //planeVAO.Unbind();
    //planeVBO.Unbind();
    
    Text systemUI = Text(StarSystem::uiFont);
    Text fpsUI = Text(StarSystem::uiFont);


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);


    glfwSwapInterval(1); // turn on vsync (limit framrate)

    double prevTime = glfwGetTime();
    int fpsDisplayRegulator = 0;
    int FPS = 0;
    // std::cout << "FPS: ";

    while(!glfwWindowShouldClose(window))
    {
        if (fpsDisplayRegulator == 10)
        {
            double currentTime = glfwGetTime();
            FPS = 10 / (currentTime - prevTime);
            prevTime = currentTime;
            fpsDisplayRegulator = 0;
        }

        fpsDisplayRegulator++;

        // std::cout << "\x1b[2K\rFPS: " << FPS;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (Systems::boundSystem != -1)
        {
            float camDistance = glm::distance(Systems::systems[Systems::boundSystem].position, camera.Position / StarSystem::scale);
            camera.speed = (camDistance <= 4e-6) ? (5e-9f * StarSystem::scale) : (glm::min(5.0f * StarSystem::scale * camDistance * camDistance + 5e-7f * StarSystem::scale, 0.02f * StarSystem::scale));
            // std::cout << "speed: " << camera.speed << " dist: " << camDistance << std::endl;
        }
        else
        {
            camera.speed = 500.0f;
        }


        camera.Inputs(window, StarSystem::scale);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetFramebufferSize(window, &camera.width, &camera.height);

        Systems::checkInfluence(camera);

        // uncomment to enable drawing the equatorial plane
        //StarSystem::defaultStarShader.Activate();
        //planeVAO.Bind();
        //// glDrawArrays(GL_TRIANGLES, 0, 6);
        //planeVAO.Unbind();

        Systems::drawAll(camera);

        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            Systems::drawLabels(camera, uiShader);
        

        systemUI.RenderText(uiShader, "Current System: " + (Systems::boundSystem != -1 ? Systems::systems[Systems::boundSystem].name : "None"), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        fpsUI.RenderText(uiShader, "FPS: " + std::to_string(FPS), 25.0f, 50.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
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


void printMarquee()
{
    int delayTime = 50;
    std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl; sleepy(delayTime);
    std::cout << "  ,=====; ;--v--; ,-----, ,-----,    ,=====< v     v ,-----, ,-----, ;--v--; " << std::endl; sleepy(delayTime);
    std::cout << " ||          |    |     | |     |   ||       |     | |     | |     |    |    " << std::endl; sleepy(delayTime);
    std::cout << "  '=====,    |    |>---<| |>,---'   ||   *   |>---<| |>---<| |>,---'    |    " << std::endl; sleepy(delayTime);
    std::cout << "       ||    |    |     | | '---,   ||       |     | |     | | '---,    |    " << std::endl; sleepy(delayTime);
    std::cout << "  ,    ||   _|_  _|_   _|_|_   _|_  ||     ,_|_   _|_|_   _|_|_   _|_  _|_   " << std::endl; sleepy(delayTime);
    std::cout << "  '=====' ~~~~~~~~~~~~~~~~~~~~~~~~   '=====' ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl; sleepy(delayTime);
    std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n" << std::endl; sleepy(delayTime);
}