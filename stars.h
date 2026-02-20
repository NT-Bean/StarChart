#ifndef STARS_CLASS_H
#define STARS_CLASS_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "camera.h"
#include "texture.h"


class Stars
{
public:
    const float pi = glm::pi<float>();
    static float scale;

    Texture flareTex = Texture("resources/Untitled.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    Shader starShader = Shader("resources/star.vert", "resources/star.frag");
    Shader flareShader = Shader("resources/flare.vert", "resources/flare.frag");

    struct Star
    {
        std::string name = "unknown star";
        float radius = 1.0f; // in solar radii
        glm::vec4 color = glm::vec4(1.0, 0.0f, 1.0f, 1.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // unit is lightyears right up until you render it

        std::vector<Vertex> vertices = {};
        std::vector<GLuint> indices = {};
        VAO starVAO;

        float luminosity = 1.0f;

        VAO flareVAO;

        Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 position, int subdivisions);

        void draw(Shader& shader, Shader& flareShader, Camera& camera, Texture flareTex);
    };

    std::vector<Star> bodies = {};

    Stars(std::vector<Star> stars, Texture flareTex, Shader starShader, Shader flareShader);

    void drawAll(Shader& starShader, Shader& flareShader, Camera& camera);
    void deleteAll();


    static std::vector<Vertex> defineSphereVertices(float radius, glm::vec4 color, glm::vec3 position, int subdivisions);
    static std::vector<GLuint> defineSphereIndices(std::vector<Vertex> vertices, int subdivisions);
};


#endif