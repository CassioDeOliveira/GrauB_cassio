#include "SpriteChar.h"

SpriteChar::SpriteChar(GLuint textureID, GLuint shaderProgram, int nAnimations, int nFrames)
    : textureID(textureID), shaderProgram(shaderProgram), pos (0.0, 0.0, 0.0), escala(0.0, 0.0, 1.0), rotation(0.0f)
{
    initRenderData();
}

void SpriteChar::initRenderData()
{

    this->offsetTex = glm::vec2(1.0f / nFrames, 1.0f / nAnimations);


    offsetTex.s = 1.0/ (float) nFrames;
	offsetTex.t = 1.0/ (float) nAnimations;

    //Especificação da geometria da sprite (quadrado, 2 triangulos)
    GLfloat vertices[] = {
	//x   y    z    r      g      b      s    t
        -0.5, 0.5, 0.0, 0.0, offsetTex.t, //v0
        -0.5,-0.5, 0.0, 0.0, 0.0, //v1
         0.5, 0.5, 0.0, offsetTex.s, offsetTex.t, //v2
        -0.5,-0.5, 0.0, 0.0, 0.0, //v1
         0.5,-0.5, 0.0, offsetTex.s, 0.0, //v3
         0.5, 0.5, 0.0, offsetTex.s, offsetTex.t  //v2
	};

    GLuint VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Atributo layout 1 - Cor - 3 valores dos 8 que descrevem o vértice
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo layout 2 - Coordenada de textura - 2 valores dos 8 que descrevem o vértice
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6* sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	FPS = 12.0;
	lastTime = 0.0;

}

void SpriteChar::setPosition(glm::vec3 pos)
{
    pos = pos;
}

glm::vec3 SpriteChar::getPosition()
{
    return pos;
}

void SpriteChar::setScale(glm::vec3 scl)
{
    escala = scl;
}

void SpriteChar::setRotation(float rot)
{
    rotation = rot;
}

void SpriteChar::setTexture(GLuint texture) {
        textureID = texture;
    }

void SpriteChar::update()
{
    float now = glfwGetTime();

	float dt = now - lastTime;

	if (dt >= 1 / FPS)
	{
		iFrame = (iFrame + 1) % nFrames; //incrementando ciclicamente o indice do Frame
		lastTime = now;
	}
	//Calculando o quanto teremos que deslocar nas coordenadas de textura
	float offsetTexFrameS = iFrame * offsetTex.s;
	float offsetTexFrameT = iAnimation * offsetTex.t;
	glUniform2f(glGetUniformLocation(shaderProgram, "offset"), offsetTexFrameS, offsetTexFrameT);
    glUniform2f(glGetUniformLocation(shaderProgram, "tileSize"), offsetTex.s, offsetTex.t);

    glm::mat4 model = glm::mat4(1); //matriz identidade
    model = glm::translate(model, pos);
    model = glm::rotate(model,glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, escala);
    glUseProgram(shaderProgram);
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
}


void SpriteChar::draw()
{
    update();

    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SpriteChar::processInput(GLFWwindow* window)
{
        // Controle de movimento com as teclas WASD ou setas
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        setPosition(getPosition() + glm::vec3(0.0f, 100.0f, 0.0f));

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        setPosition(getPosition() + glm::vec3(0.0f, -100.0f, 0.0f));

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        setPosition(getPosition() + glm::vec3(-100.0f, 0.0f, 0.0f));

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        setPosition(getPosition() + glm::vec3(100.0f, 0.0f, 0.0f));


        update();
}



