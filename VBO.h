#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>

struct Vertex
{
    glm::vec3 position;
    // glm::vec3 normal;
    glm::vec4 color;
};
class VBO
{
public:

    GLuint vboID;


    VBO(std::vector<Vertex>& vertices);

    void Bind();
    void Unbind();
    void Delete();
    
};

#endif