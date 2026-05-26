#include "systems.h"

typedef StarSystem::Star Star;
typedef StarSystem::AstroCoords AstroCoords;

int Systems::boundSystem = -1;
std::vector<StarSystem> Systems::systems;

void Systems::init()
{
    Systems::systems = std::vector<StarSystem>{

        StarSystem(std::vector<Star> {
            //   name   rad.  temp. lumin.      astronomical coordinates    dist.  # of subdivisions
            Star("Sol", 1.0f, 5772, 1.0f, AstroCoords(0, 0, 0.0, 0, 0, 0.0, 0.0f), 128)
                //  name           astronomical coordinates          dist.  influence radius
                }, "Solar System", AstroCoords(AstroCoords::Centralized), 0.1f),


            // List of nearest stars - Wikipedia (https://en.wikipedia.org/wiki/List_of_nearest_stars)

            StarSystem(std::vector<Star> {
                Star("Alpha Centauri A", 1.2175f, 5804, 1.5059f, AstroCoords(14, 39, 36.494, -60, 50, 2.3737, 4.344f), 128),
                Star("Alpha Centauri B", 0.8591f, 5207, 0.4981f, AstroCoords(14, 39, 36.06311, -60, 50, 15.0992, 4.344f), 128),
                Star("Proxima Centauri", 0.1542f, 2992, 0.00005f, AstroCoords(14, 29, 42.946, -62, 40, 46.16, 4.2465f), 96)
            }, "Alpha Centauri", AstroCoords(AstroCoords::AverageTwo), 0.3f),

            StarSystem(std::vector<Star> {
                Star("Barnard's Star", 0.187f, 3195, 0.0004f, AstroCoords(17, 57, 48.49847, 4, 41, 36.1139, 5.9629f), 96)
            }, "Barnard's Star (sys)", AstroCoords(AstroCoords::Centralized), 0.05f),

            StarSystem(std::vector<Star> {
                Star("Sirius A", 1.7144f, 9845, 24.74f, AstroCoords(6, 45, 08.917, -16, 42, 58.02, 8.61f), 128),
                Star("Sirius B", 0.008098f, 25000, 0.0f, AstroCoords(6, 45, 9.0, -16, 43, 6.0, 8.61f), 64)
            }, "Sirius", AstroCoords(AstroCoords::Centralized), 0.1f),

            StarSystem(std::vector<Star> {
                Star("Epsilon Eridani", 0.738f, 5085, 0.32f, AstroCoords(3, 32, 55.84496, -9, 27, 29.7312f, 10.475f), 128)
            }, "Epsilon Eridani (sys)", AstroCoords(AstroCoords::Centralized), 0.1f),

            StarSystem(std::vector<Star> {
                Star("61 Cyg A", 0.667f, 4398, 0.15f, AstroCoords(21, 6, 53.9396, 38, 44, 57.902, 11.404f), 96),
                Star("61 Cyg B", 0.594f, 4174, 0.097f, AstroCoords(21, 6, 55.2638, 38, 44, 31.359, 11.404f), 96)
            }, "61 Cygni", AstroCoords(AstroCoords::Averaged), 0.1f),

            StarSystem(std::vector<Star> {
                Star("Procyon A", 2.043f, 6582, 7.049f, AstroCoords(7, 39, 18.1195, 5, 13, 29.9552, 11.46f), 128),
                Star("Procyon B", 0.01234f, 7740, 0.0f, glm::vec3(0.0003054538f, 0.0f, 0.0f), 64)
            }, "Procyon", AstroCoords(AstroCoords::Centralized), 0.1f),

            StarSystem(std::vector<Star> {
                Star("Epsilon Indi A", 0.713f, 4649, 0.21f, AstroCoords(22, 3, 21.65363, -56, 47, 9.5228, 11.867f), 128),
                Star("Epsilon Indi Ba", 0.08f, 1312, 0.0f, AstroCoords(22, 10, 3.65363, -56, 47, 9.5228, 11.867f), 64),
                Star("Epsilon Indi Bb", 0.082f, 972, 0.0f, AstroCoords(22, 10, 3.65363, -56, 47, 8.7928, 11.867f), 64)
            }, "Epsilon Indi", AstroCoords(AstroCoords::Centralized), 0.3f),

            StarSystem(std::vector<Star> {
                Star("Tau Ceti", 0.793f, 5320, 0.45f, AstroCoords(1, 44, 4.083, -15, 56, 14.93, 11.912f), 128)
            }, "Tau Ceti (sys)", AstroCoords(AstroCoords::Centralized), 0.1f),

            StarSystem(std::vector<Star> {
                Star("Groombridge 1618", 0.605f, 3970, 0.049f, AstroCoords(10, 11, 22.13995, 49, 27, 15.2510, 15.886f), 96)
            }, "Groombridge 1618 (sys)", AstroCoords(AstroCoords::Centralized), 0.1f),

            StarSystem(std::vector<Star> {
                Star("40 Eridani A", 0.804f, 5126, 0.4f, AstroCoords(4, 15, 16.31962, -7, 39, 10.3308, 16.360f), 128),
                Star("40 Eridani B", 0.01308f, 17200, 0.0f, AstroCoords(4, 15, 21.79572, -7, 39, 29.2040, 16.333f), 64),
                Star("40 Eridani C", 0.274f, 3167, 0.0f, AstroCoords(4, 15, 21.53600, -7, 39, 20.6946, 16.353f), 96)
            }, "40 Eridani", AstroCoords(AstroCoords::Centralized), 1.0f)
    };
}


void Systems::checkInfluence(Camera& camera)
{
    if (Systems::boundSystem == -1)
    {
        for (int i = 0; i < Systems::systems.size(); i++)
        {
            if (glm::distance(camera.Position / StarSystem::scale, Systems::systems[i].position) < StarSystem::influenceRadius)
            {
                Systems::boundSystem = i;
                std::cout << "\njust bound to " << Systems::systems[Systems::boundSystem].name << " (" << Systems::boundSystem << ") buddeh" << std::endl;

                camera.Position -= Systems::systems[Systems::boundSystem].position * StarSystem::scale;
                glm::vec3 oldPos = Systems::systems[Systems::boundSystem].position;
                // std::cout << "oldPos (" << oldPos.x << ", " << oldPos.y << ", " << oldPos.z << ")" << std::endl;

                for (int j = 0; j < Systems::systems.size(); j++)
                {
                    Systems::systems[j].position -= oldPos;
                    // std::cout << Systems::systems[j].name << " new pos (" << Systems::systems[j].position.x << ", " << Systems::systems[j].position.y << ", " << Systems::systems[j].position.z << ")" << std::endl;
                    
                    Systems::systems[j].defineSystem();
                }
                return;
            }
        }
    }
    else if (Systems::boundSystem == 0 && glm::distance(camera.Position / StarSystem::scale, Systems::systems[0].position) > Systems::systems[0].influenceRadius)
    {
        Systems::boundSystem = -1;
        std::cout << "\nstar 0 was unbound from. ...wait, what was the point of that again?" << std::endl;
    }
    else if (glm::distance(camera.Position / StarSystem::scale, Systems::systems[Systems::boundSystem].position) > Systems::systems[boundSystem].influenceRadius)
    {
        std::cout << '\n' << Systems::systems[Systems::boundSystem].name << " (" << Systems::boundSystem << ") was unbound from. you are now floating in the interstellar medium" << std::endl;
        Systems::boundSystem = -1;
        camera.Position -= Systems::systems[0].position * StarSystem::scale;
        for (int i = 1; i < Systems::systems.size(); i++)
        {
            Systems::systems[i].position -= Systems::systems[0].position;
            Systems::systems[i].defineSystem();
        }
        Systems::systems[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
        Systems::systems[0].defineSystem();
        return;
    }
}


void Systems::drawLabels(Camera& camera, Shader uiShader)
{

    for (int i = 0; i < Systems::systems.size(); i++)
    {
        for (int j = 0; j < Systems::systems[i].bodies.size(); j++)
        {
            float farPlane = 1e4f * StarSystem::scale;
            float nearPlane = glm::distance(camera.Position / StarSystem::scale, glm::vec3(0.0f, 0.0f, 0.0f)) <= 1e2f * StarSystem::scale ? 1e-2f * StarSystem::scale : 1e2f * StarSystem::scale;

            glm::vec3 truePosition = Systems::systems[i].bodies[j].absolutePos * StarSystem::scale;
            glm::vec3 trueCamPos = camera.Position * StarSystem::scale;

            glm::vec3 screenCoords = glm::project(truePosition, camera.GetViewMatrix(30.0f, nearPlane, farPlane), camera.GetProjectionMatrix(30.0f, nearPlane, farPlane), glm::vec4(0, 0, 1280, 720));

            if (screenCoords.z < 0.0f || screenCoords.z > 1.0f)
                continue;

            const char* text2print;

            if (Systems::boundSystem == i)
            {
                text2print = Systems::systems[i].bodies[j].name.c_str();
            }
            else
            {
                text2print = Systems::systems[i].name.c_str();
                if (j != 0)
                    continue;
            }

            Systems::systems[i].bodies[j].starLabel.RenderText(uiShader, text2print, screenCoords.x + 10, screenCoords.y + 10, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
}


void Systems::drawAll(Camera& camera)
{
    for (int i = 0; i < Systems::systems.size(); i++)
    {
        Systems::systems[i].drawSystem(camera);
    }
}

void Systems::deleteAll()
{
    for (int i = 0; i < Systems::systems.size(); i++)
    {
        Systems::systems[i].deleteSystem();
    }
    StarSystem::defaultFlareTex.Delete();
    StarSystem::defaultFlareShader.Delete();
    StarSystem::defaultStarShader.Delete();
}