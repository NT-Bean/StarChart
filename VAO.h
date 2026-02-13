#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:

    GLuint vaoID;


    VAO();

    void LinkAttribute(VBO& VBO, GLuint shaderIndex, GLint numberOfComponents, GLenum dataType, GLsizei vertexSize, int offset);
    void Bind();
    void Unbind();
    void Delete();

};

#endif