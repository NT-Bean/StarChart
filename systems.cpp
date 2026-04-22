#include "systems.h"

typedef StarSystem::Star Star;

int Systems::boundSystem = -1;
std::vector<StarSystem> Systems::systems;

void Systems::init(std::vector<StarSystem> systems)
{
    Systems::systems = systems;
}

void Systems::drawAll(Camera& camera)
{
    for (int i = 0; i < Systems::systems.size(); i++)
    {
        Systems::systems[i].drawSystem(camera);
    }
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
                std::cout << "just bound to " << Systems::systems[Systems::boundSystem].name << " (" << Systems::boundSystem << ") buddeh" << std::endl;

                camera.Position -= Systems::systems[Systems::boundSystem].position * StarSystem::scale;
                glm::vec3 oldPos = Systems::systems[Systems::boundSystem].position;
                std::cout << "oldPos (" << oldPos.x << ", " << oldPos.y << ", " << oldPos.z << ")" << std::endl;

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
        std::cout << "star 0 was unbound from. ...wait, what was the point of that again?" << std::endl;
    }
    else if (glm::distance(camera.Position / StarSystem::scale, Systems::systems[Systems::boundSystem].position) > Systems::systems[boundSystem].influenceRadius)
    {
        std::cout << Systems::systems[Systems::boundSystem].name << " (" << Systems::boundSystem << ") was unbound from. you are now floating in the interstellar medium" << std::endl;
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