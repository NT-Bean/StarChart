#ifndef STAR_SYSTEM_CLASS_H
#define STAR_SYSTEM_CLASS_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <string>

#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "camera.h"
#include "texture.h"

class StarSystem
{
public:
    std::string name = "unknown system";
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is lightyears right up until you render it

    static float scale;
    static bool renderFlares;
    static Shader defaultStarShader;
    static Shader defaultFlareShader;
    static Texture defaultFlareTex;

    int boundStar = -1;
    static float influenceRadius;

    Texture flareTex = Texture(NULL);
    Shader starShader = Shader(NULL);
    Shader flareShader = Shader(NULL);

    struct AstroCoords
    {
        double raDecimal;
        double decDecimal;
        float distance;

        AstroCoords(int raHours, int raMinutes, double raSeconds, int decDegrees, int decArcminutes, double decArcseconds, float distance);
        glm::vec3 ToPosition();
    };

    struct Star
    {
        StarSystem* system = NULL;

        std::string name = "unknown star";
        float radius = 1.0f; // in solar radii
        glm::vec3 color = glm::vec3(1.0, 0.0f, 1.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is lightyears right up until you render it
        glm::vec3 absolutePos = glm::vec3(0.0f, 0.0f, 0.0f);

        int subdivisions = 128;

        std::vector<Vertex> vertices = {};
        std::vector<GLuint> indices = {};
        VAO starVAO;

        float luminosity = 1.0f;

        VAO flareVAO;

        Star(std::string name, float radius, float temperature, float luminosity, glm::vec3 pos, int subdivisions);
        Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 pos, int subdivisions);

        void define(int subdivisions);
        void draw(Shader& shader, Shader& flareShader, Camera& camera, Texture flareTex);
    };


    std::vector<Star> bodies = {};

    StarSystem(std::vector<Star> stars, std::string name, AstroCoords astroCoords, Texture flareTex, Shader starShader, Shader flareShader, float bindingRadius);

    void drawSystem(Camera& camera);
    void defineSystem();
    void deleteSystem();

    static std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions);
    static std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions);
};


#endif