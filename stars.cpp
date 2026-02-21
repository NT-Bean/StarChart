#include "stars.h"

float Stars::scale = 1e4f;
float Stars::influenceRadius = 0.1f;

Stars::Star::Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 position, int subdivisions)
{

    this->name = name;
    this->radius = radius;
    this->color = glm::vec4(color, 1.0f);
    this->luminosity = luminosity;
    this->position = position;

    std::cout << "New star generated named " << name << std::endl;
    std::cout << name << "'s radius (R_sol) is: " << radius << std::endl;

    std::cout << name << " is located at (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")" << std::endl;
    std::cout << name << "'s scale pos is (" << this->position.x * scale << ", " << this->position.y * scale << ", " << this->position.z * scale << ")" << std::endl;

    std::cout << "the luminosity of " << name << " is " << luminosity << std::endl;

    define(subdivisions);
}

void Stars::Star::define(int subdivisions)
{
    if (subdivisions % 4 != 0 || subdivisions < 12)
    {
        subdivisions = 16;
    }

    vertices = defineSphereVertices(radius, color, position, subdivisions);
    indices = defineSphereIndices(vertices, subdivisions);

    starVAO.Bind();
    VBO starVBO(vertices);
    EBO starEBO(indices);

    starVAO.LinkAttribute(starVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    starVAO.LinkAttribute(starVBO, 1, 4, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    starVAO.Unbind();
    starVBO.Unbind();
    starEBO.Unbind();

    flareVAO.Bind();

    std::vector<Vertex> point = { Vertex(position * scale, color) };
    VBO flareVBO(point);

    flareVAO.LinkAttribute(flareVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    flareVAO.LinkAttribute(flareVBO, 1, 4, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    flareVAO.Unbind();
    flareVBO.Unbind();
}

void Stars::Star::draw(Shader& shader, Shader& flareShader, Camera& camera, Texture flareTex)
{
    float distance = glm::distance(position, camera.Position / scale) * 299792458.0f * 86400 * 365; // converting scaled distance to meters
    float irradiance = (luminosity * 3.827e26f) / (4 * glm::pi<float>() * distance * distance); // converts luminosity to watts inside equation


    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    std::vector<Shader> shaders = { flareShader, shader };
    camera.Matrix(60.0f, 1e-7 * scale, 1e6f * scale, shaders, "camMatrix");

    shader.Activate();
    starVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    starVAO.Unbind();

    flareShader.Activate();
    glUniform1f(glGetUniformLocation(flareShader.ID, "irradiance"), irradiance);
    glUniform1f(glGetUniformLocation(flareShader.ID, "luminosity"), luminosity);
    // std::cout << name << " evirradiance " << irradiance + glm::max(6.0f * glm::sqrt(luminosity), 5.0f) << " watts" << std::endl;
    // std::cout << name << " distance is " << distance << " meters" << std::endl;
    // std::cout << "this star is at (" << position.x * scale << ", " << position.y * scale << ", " << position.z * scale << ")" << std::endl;
    // std::cout << "camera position is (" << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << ")" << std::endl << std::endl;

    flareVAO.Bind();

    glActiveTexture(GL_TEXTURE0);
    flareTex.Bind();
    flareTex.texUnit(flareShader, "tex0", 0);

    glDrawArrays(GL_POINTS, 0, 1);
    flareVAO.Unbind();
}



Stars::Stars(std::vector<Star> bodies, Texture flareTex, Shader starShader, Shader flareShader, float influenceRadius)
{
    this->stars = bodies;
    this->flareTex = flareTex;
    this->starShader = starShader;
    this->flareShader = flareShader;
    this->influenceRadius = influenceRadius;
}

void Stars::checkInfluence(Camera& camera)
{
    if (boundStar == -1)
    {
        for (int i = 0; i < stars.size(); i++)
        {
            if (glm::distance(camera.Position / scale, stars[i].position) < influenceRadius)
            {
                boundStar = i;
                std::cout << "just bound to " << stars[boundStar].name << " (" << boundStar << ") buddeh" << std::endl;

                camera.Position -= stars[boundStar].position * scale;
                glm::vec3 oldPos = stars[boundStar].position;
                for (int j = 0; j < stars.size(); j++)
                {
                    stars[j].position -= oldPos;
                    stars[j].define(128);
                }
                return;
            }
        }
    }
    else if (boundStar == 0 && glm::distance(camera.Position / scale, stars[0].position) > influenceRadius)
    {
        std::cout << "star 0 was unbound from. ...wait, what was the point of that again?" << std::endl;
        boundStar = -1;
    }
    else if (glm::distance(camera.Position / scale, stars[boundStar].position) > influenceRadius)
    {
        std::cout << stars[boundStar].name << " (" << boundStar << ") was unbound from" << std::endl;
        boundStar = -1;
        camera.Position -= stars[0].position * scale;
        for (int i = 1; i < stars.size(); i++)
        {
            stars[i].position -= stars[0].position;
            stars[i].define(128);
        }
        stars[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
        stars[0].define(128);
        return;
    }
}

void Stars::drawAll(Shader& starShader, Shader& flareShader, Camera& camera)
{
    for (int i = 0; i < stars.size(); i++)
    {
        stars[i].draw(starShader, flareShader, camera, flareTex);
    }
}

void Stars::deleteAll()
{
    for (int i = 0; i < stars.size(); i++)
    {
        stars[i].starVAO.Delete();
        stars[i].flareVAO.Delete();
    }
}


std::vector<Vertex> Stars::defineSphereVertices(float radius, glm::vec4 color, glm::vec3 position, int subdivisions)
{
    std::vector<Vertex> vertices = {};
    constexpr float pi = glm::pi<float>();

    double radiusLy = radius * 7.35355e-8 * scale; // converts solar radius to whatever the scale is in lightyears
    std::cout << "generating points. radiusLy is defined as " << radiusLy << std::endl;

    for (int i = 0; i < subdivisions; i++) // [0...15]
    {
        float heading = ((float)i * 2.0f * pi) / (float)subdivisions;
        for (int j = 0; j <= subdivisions / 2; j++) // [0...d/2]
        {
            float azimuth = ((float)j * 2.0f * pi) / (float)subdivisions + (pi / 2);
            Vertex currentVertex =
                Vertex(glm::vec3(
                    radiusLy * glm::cos(heading) * glm::cos(azimuth) + position.x * scale,
                    radiusLy * glm::sin(azimuth) + position.y * scale,
                    radiusLy * glm::sin(heading) * glm::cos(azimuth) + position.z * scale

                ), color);
            vertices.push_back(currentVertex);
        }
    }

    return vertices;
}


std::vector<GLuint> Stars::defineSphereIndices(std::vector<Vertex> vertices, int subdivisions)
{
    std::vector<GLuint> indices = {};
    int layerCount = subdivisions / 2 + 1;
    for (int i = 1; i <= subdivisions - ((subdivisions - 1) % 2); i += 2) // horizontal
    {
        for (int j = 1; j <= layerCount - 2; j += 2) // vertical
        {
            int index = (j + layerCount * i);
            int overflowCheckedIndex = (index + layerCount > subdivisions * layerCount) ? (index + layerCount - subdivisions * layerCount) : (index + layerCount);
            int underflowCheckedIndex = (index - layerCount < 1) ? (index - layerCount + subdivisions * layerCount) : (index - layerCount);

            // top right
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(overflowCheckedIndex);

            // bottom left
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(underflowCheckedIndex);

            // top left
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(underflowCheckedIndex);

            // bottom right
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(overflowCheckedIndex);
        }
    }

    for (int i = 0; i <= (subdivisions - 2) - (subdivisions - 4) % 2; i += 2) // horizontal
    {
        for (int j = 2; j <= layerCount - 3; j += 2) // vertical
        {
            int index = (j + layerCount * i);
            int overflowCheckedIndex = (index + layerCount > subdivisions * layerCount) ? (index + layerCount - subdivisions * layerCount) : (index + layerCount);
            int underflowCheckedIndex = (index - layerCount < 1) ? (index - layerCount + subdivisions * layerCount) : (index - layerCount);

            // top right
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(overflowCheckedIndex);

            // bottom left
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(underflowCheckedIndex);

            // top left
            indices.push_back(index);
            indices.push_back(index - 1);
            indices.push_back(underflowCheckedIndex);

            // bottom right
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(overflowCheckedIndex);
        }
    }

    return indices;
}