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

        Texture flareTex;
        Shader starShader, flareShader;

        std::string name = "unknown star";
        float radius = 1.0f; // in solar radii
        glm::vec3 color = glm::vec3(1.0, 0.0f, 1.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is lightyears right up until you render it
        glm::vec3 absolutePos = glm::vec3(0.0f, 0.0f, 0.0f); // misnomer; should really be called "renderPosition" but i'm scared i'm gonna confuse myself more

        bool posIsRelative = true;
        bool isFirstDefinition = true;

        int subdivisions = 128;

        std::vector<Vertex> vertices = {};
        std::vector<GLuint> indices = {};
        VAO starVAO;

        float luminosity = 1.0f;

        VAO flareVAO;

        
        Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 pos, int subdivisions);
        Star(std::string name, float radius, int temperature, float luminosity, glm::vec3 pos, int subdivisions);

        Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 pos, int subdivisions, Shader starShader, Shader flareShader, Texture flareTex);
        Star(std::string name, float radius, int temperature, float luminosity, glm::vec3 pos, int subdivisions, Shader starShader, Shader flareShader, Texture flareTex);
            
        Star(std::string name, float radius, glm::vec3 color, float luminosity, AstroCoords astroCoords, int subdivisions);
        Star(std::string name, float radius, int temperature, float luminosity, AstroCoords astroCoords, int subdivisions);

        Star(std::string name, float radius, glm::vec3 color, float luminosity, AstroCoords astroCoords, int subdivisions, Shader starShader, Shader flareShader, Texture flareTex);
        Star(std::string name, float radius, int temperature, float luminosity, AstroCoords astroCoords, int subdivisions, Shader starShader, Shader flareShader, Texture flareTex);

        void logInit();
        void define(int subdivisions);
        void draw(Shader starShader, Shader flareShader, Texture flareTex, Camera& camera);
    };


    std::vector<Star> bodies = {};

    StarSystem(std::vector<Star> stars, std::string name, AstroCoords astroCoords, float bindingRadius);

    void drawSystem(Camera& camera);
    void defineSystem();
    void deleteSystem();


    static glm::vec3 surfaceTempToColor(float temperature);
    static std::vector<Vertex> defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions);
    static std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions);
};


#endif