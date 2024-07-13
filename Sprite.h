#ifndef SPRITE_H
#define SPRITE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sprite {
public:
    GLuint VAO;
    GLuint textureID;
    glm::vec2 position;
    glm::vec2 scale;
    float rotation, x, y, sx, sy;
    GLuint shaderProgram;

    Sprite(GLuint vao, GLuint shaderProgram);
    void setPosition(float x, float y);
    void setScale(float sx, float sy);
    void setRotation(float rot);
    void setTexture(GLuint texture);
    void update();
    void draw();

private:
    glm::mat4 model;
    void initRenderData();
};

#endif // SPRITE_H

