#include "VAO.h"

VAO::VAO()
{
    glGenVertexArrays(1, &vaoID);
}


void VAO::LinkAttribute(VBO& VBO, GLuint shaderIndex, GLint numberOfComponents, GLenum dataType, GLsizei vertexSize, int offset)
{
    VBO.Bind();
    glVertexAttribPointer(shaderIndex, numberOfComponents, dataType, GL_FALSE, vertexSize, (void*)offset);
    glEnableVertexAttribArray(shaderIndex);
    VBO.Unbind();
}

void VAO::Bind()
{
    glBindVertexArray(vaoID);
}


void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::Delete()
{
    glDeleteVertexArrays(1, &vaoID);
}