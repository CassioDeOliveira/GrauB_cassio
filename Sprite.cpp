#include "Sprite.h"



    Sprite::Sprite(GLuint vao, GLuint shaderProgram)
        : VAO(vao), shaderProgram(shaderProgram), textureID(0), position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0f) {}

    void Sprite::setPosition(float x, float y) {
        position = glm::vec2(x, y);
    }

    void Sprite::setScale(float sx, float sy) {
        scale = glm::vec2(sx, sy);
    }

    void Sprite::setRotation(float angle) {
        rotation = angle;
    }

    void Sprite::setTexture(GLuint texture) {
        textureID = texture;
    }


    void Sprite::draw() {
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }



