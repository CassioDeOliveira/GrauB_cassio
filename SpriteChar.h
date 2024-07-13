#ifndef SPRITECHAR_H
#define SPRITECHAR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SpriteChar {
public:
    GLuint VAO;
    GLuint textureID;
    glm::vec3 pos;
    glm::vec3 escala;
    float rotation;
    GLuint shaderProgram;
    int nAnimations, nFrames, iAnimation, iFrame;
    glm::vec2 offsetTex; //ds e dt
    float lastTime;
    float FPS;


    SpriteChar(GLuint textureID, GLuint shaderProgram, int nAnimations, int nFrames);
    void setPosition(glm::vec3 pos);
    glm::vec3 getPosition();
    void setScale(glm::vec3 scl);
    void setRotation(float rot);
    void setTexture(GLuint texture);
    void update();
    void draw();
    void processInput(GLFWwindow* window);

private:
    glm::mat4 model;
    void initRenderData();
};


#endif // SPRITECHAR_H
