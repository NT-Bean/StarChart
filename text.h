#ifndef TEXT_CLASS_H
#define TEXT_CLASS_H

#include "font.h"
#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Text
{
public:
    Font font;
    unsigned int VAO1, VBO1;

    // bool isFirstRender = true;

    glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

    Text(Font font);
    Text();

    void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);
};

#endif