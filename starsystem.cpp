#include "starsystem.h"

typedef StarSystem::Star Star;
typedef StarSystem::AstroCoords AstroCoords;

float StarSystem::scale = 1e4f;
Shader StarSystem::defaultStarShader = Shader();
Shader StarSystem::defaultFlareShader = Shader();
Texture StarSystem::defaultFlareTex = Texture();

bool StarSystem::renderFlares = true;

float StarSystem::influenceRadius = 0.1f;


AstroCoords::AstroCoords(int raHours, int raMinutes, double raSeconds, int decDegrees, int decArcminutes, double decArcseconds, float distance)
{
    this->distance = distance;

    this->raDecimal = (double)raHours + (double)raMinutes / 60.0f + raSeconds / 3600.0f;
    // std::cout << "right ass " << raDecimal << std::endl;
    this->decDecimal = (double)decDegrees + (double)decArcminutes / 60.0f + decArcseconds / 3600.0f;
    // std::cout << "defamation " << decDecimal << std::endl;

}
glm::vec3 AstroCoords::ToPosition()
{
    double raRad = glm::radians<double>(raDecimal * 15);
    double decRad = glm::radians<double>(decDecimal);

    float x = distance * glm::cos(decRad) * glm::cos(raRad);
    float y = distance * glm::sin(decRad);
    float z = distance * glm::cos(decRad) * glm::sin(raRad);

    return glm::vec3(x, y, z);
}

Star::Star(std::string name, float radius, float temperature, float luminosity, glm::vec3 pos, int subdivisions)
{

    this->name = name;
    this->radius = radius;
    this->luminosity = luminosity;

    this->position = pos; // astroCoods.ToPosition();

    temperature /= 100.0f;
    float red = 255.0f;
    float green = 0.0f;
    float blue = 255.0f;

    if (temperature <= 66.0f)
    {
        red = 255.0f;
        green = glm::clamp(99.471f * glm::log(temperature) - 161.12f, 0.0f, 255.0f);
    }
    else
    {
        red = temperature - 60.0f;
        red = glm::clamp(329.7f * (glm::pow(red, -0.1332f)), 0.0f, 255.0f);
        green = temperature - 60;
        green = glm::clamp(288.122f * glm::pow(green, -0.0755f), 0.0f, 255.0f);
    }

    if (temperature >= 66.0f)
    {
        blue = 255.0f;
    }
    else if (temperature <= 19.0f)
    {
        blue = 0.0f;
    }
    else
    {
        blue = temperature - 10.0f;
        blue = 138.51773f * glm::log(blue) - 305.0448f;
    }
    this->color = glm::vec3(red / 255.0f, green / 255.0f, blue / 255.0f);


    std::cout << "New star generated named " << name << std::endl;
    std::cout << name << "'s radius (R_sol) is: " << radius << std::endl;
    std::cout << "The color of " << name << " is (" << red << ", " << green << ", " << blue << ")" << std::endl;

    std::cout << name << " is located at (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")" << std::endl;
    std::cout << name << "'s scale pos is (" << this->position.x * scale << ", " << this->position.y * scale << ", " << this->position.z * scale << ")" << std::endl;

    std::cout << "The luminosity of " << name << " is " << luminosity << std::endl;

    define(subdivisions);

    std::cout << std::endl;
}
Star::Star(std::string name, float radius, glm::vec3 color, float luminosity, glm::vec3 pos, int subdivisions)
{
    this->name = name;
    this->radius = radius;
    this->luminosity = luminosity;

    this->position = pos; // astroCoords.ToPosition();

    this->color = color;


    std::cout << "New star generated named " << name << std::endl;
    std::cout << name << "'s radius (R_sol) is: " << radius << std::endl;
    std::cout << "The color of " << name << " is (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;

    std::cout << name << " is located at (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")" << std::endl;
    std::cout << name << "'s scale pos is (" << this->position.x * scale << ", " << this->position.y * scale << ", " << this->position.z * scale << ")" << std::endl;

    std::cout << "The luminosity of " << name << " is " << luminosity << std::endl;

    define(subdivisions);

    std::cout << std::endl;
}


void Star::define(int subdivisions)
{
    if (subdivisions % 4 != 0 || subdivisions < 12)
    {
        subdivisions = 16;
    }

    if (system != NULL) { absolutePos = position + system->position; }
    else { absolutePos = position; }

    /*
    // std::cout << "tamam keyboard (" << absolutePos.x << ", " << absolutePos.y << ", " << absolutePos.z << ")" << std::endl;
    std::cout << "mine doesnt do that keyboard (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    if (system != NULL)
    {
        std::cout << "hellp theres a woman talking in ear about no shit (" << system->position.x << ", " << system->position.y << ", " << system->position.z << ")" << std::endl;
    }
    */

    vertices = defineSphereVertices(radius, color, absolutePos, subdivisions);
    indices = defineSphereIndices(vertices, subdivisions);

    starVAO.Bind();
    VBO starVBO(vertices);
    EBO starEBO(indices);

    unsigned long long whatIsThisDataType = 0;

    starVAO.LinkAttribute(starVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    starVAO.LinkAttribute(starVBO, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    starVAO.Unbind();
    starVBO.Unbind();
    starEBO.Unbind();

    flareVAO.Bind();

    std::vector<Vertex> point = { Vertex(absolutePos * scale, color) };
    VBO flareVBO(point);

    flareVAO.LinkAttribute(flareVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    flareVAO.LinkAttribute(flareVBO, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));

    flareVAO.Unbind();
    flareVBO.Unbind();
}

void Star::draw(Shader& shader, Shader& flareShader, Camera& camera, Texture flareTex)
{
    if (system != NULL) { absolutePos = position + system->position; }
    else { absolutePos = position; }

    float distance = glm::distance(absolutePos, camera.Position / scale) * 299792458.0f * 86400 * 365; // converting scaled distance to meters
    float irradiance = (luminosity * 3.827e26f) / (4 * glm::pi<float>() * distance * distance); // converts luminosity to watts inside equation


    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    std::vector<Shader> shaders = { flareShader, shader };

    float nearPlane = glm::distance(position * scale, camera.Position) < 1e-6 * scale ? 1e-13 * scale : 1e-6 * scale;

    camera.Matrix(60.0f, nearPlane, 1e6f * scale, shaders, "camMatrix");

    shader.Activate();
    starVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    starVAO.Unbind();

    flareShader.Activate();
    glUniform1f(glGetUniformLocation(flareShader.ID, "irradiance"), irradiance);
    glUniform1f(glGetUniformLocation(flareShader.ID, "luminosity"), luminosity);
    
    // std::cout << name << " distance is " << distance << " meters" << std::endl;
    // std::cout << "this star is at (" << position.x * scale << ", " << position.y * scale << ", " << position.z * scale << ")" << std::endl;
    // std::cout << "camera position is (" << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << ")" << std::endl << std::endl; 

    flareVAO.Bind();

    glActiveTexture(GL_TEXTURE0);
    flareTex.Bind();
    flareTex.texUnit(flareShader, "tex0", 0);

    if(renderFlares) { glDrawArrays(GL_POINTS, 0, 1); }
    flareVAO.Unbind();
}


StarSystem::StarSystem(std::vector<Star> bodies, std::string name, AstroCoords astroCoords, Texture flareTex, Shader starShader, Shader flareShader, float influenceRadius)
{
    this->bodies = bodies;
    this->name = name;
    this->position = astroCoords.ToPosition();
    this->flareTex = flareTex;
    this->starShader = starShader;
    this->flareShader = flareShader;
    this->influenceRadius = influenceRadius;

    for (int i = 0; i < bodies.size(); i++)
    {
        this->bodies[i].system = this;
        std::cout << "setting system of " << this->bodies[i].name << " to " << this << std::endl;
    }
}

void StarSystem::defineSystem()
{
    for (int i = 0; i < bodies.size(); i++)
    {
        bodies[i].system = this;
        bodies[i].define(bodies[i].subdivisions);
    }
}

void StarSystem::drawSystem(Camera& camera)
{
    for (int i = 0; i < bodies.size(); i++)
    {
        bodies[i].draw(
            starShader.ID != NULL ? starShader : StarSystem::defaultStarShader,
            flareShader.ID != NULL ? flareShader : StarSystem::defaultFlareShader,
            camera,
            flareTex.ID != NULL ? flareTex : StarSystem::defaultFlareTex
        );
    }
}

void StarSystem::deleteSystem()
{
    for (int i = 0; i < bodies.size(); i++)
    {
        bodies[i].starVAO.Delete();
        bodies[i].flareVAO.Delete();
    }
}



std::vector<Vertex> StarSystem::defineSphereVertices(float radius, glm::vec3 color, glm::vec3 position, int subdivisions)
{
    std::vector<Vertex> vertices = {};

    double radiusLy = radius * 7.35355e-8 * scale; // converts solar radius to whatever the scale is in lightyears
    std::cout << "generating points. radiusLy is defined as " << radiusLy << std::endl;

    constexpr float pi = glm::pi<float>();

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

std::vector<GLuint> StarSystem::defineSphereIndices(std::vector<Vertex> vertices, int subdivisions)
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