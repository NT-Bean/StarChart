#ifndef STARS_CLASS_H
#define STARS_CLASS_H

#include "starsystem.h"

class Systems
{
public:
    static int boundSystem;

    static std::vector<StarSystem> systems;

    static void init(std::vector<StarSystem> systems);

    static void checkInfluence(Camera& camera);

    static void drawAll(Camera& camera);
    static void deleteAll();
};

#endif