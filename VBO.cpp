#include "VBO.h"

VBO::VBO(std::vector<Vertex> &vertices)
{
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
}
void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
}

// Unbinds the VBO
void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete()
{
    glDeleteBuffers(1, &vboID);
}