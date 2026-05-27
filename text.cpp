#include "text.h"

typedef Font::Character Character;

Text::Text(Font font)
{
    this->font = font;

    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //if (this->font.Characters.empty() && StarSystem::verboseLog) {
    //    std::cout << "ERROR: No characters loaded in font!" << std::endl;
    //} else if (StarSystem::verboseLog){
    //    std::cout << "Loaded " << this->font.Characters.size() << " characters." << std::endl;
    //}
}
Text::Text()
{
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //if (uiFont.Characters.empty() && StarSystem::verboseLog) {
    //    std::cout << "ERROR: No characters loaded in font!" << std::endl;
    //}
    //else if (StarSystem::verboseLog) {
    //    std::cout << "Loaded " << uiFont.Characters.size() << " characters." << std::endl;
    //}
}


void Text::RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color)
{
    //if (isFirstRender)
    //{
    //    isFirstRender = false;
    //    return;
    //}

    glDisable(GL_DEPTH_TEST);

    // activate corresponding render state	
    s.Activate();

    GLint colorLoc = glGetUniformLocation(s.ID, "textColor");
    GLint projLoc = glGetUniformLocation(s.ID, "projection");

    //if(StarSystem::verboseLog)
    //    std::cout << "Uniform Locations - textColor: " << colorLoc << ", projection: " << projLoc << std::endl;

    glUniform3f(colorLoc, color.x, color.y, color.z);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(s.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);


    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO1);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        if (font.Characters.find(*c) != font.Characters.end()) {
            Character ch = font.Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO1);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        } else {
            // Handle the case where the character is not found
            continue;
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
}