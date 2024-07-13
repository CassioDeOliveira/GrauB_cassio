#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Sprite.h"
#include "SpriteChar.h"
#define TILE_WIDTH 64
#define TILE_HEIGHT 32

using namespace std;
namespace fs = std::filesystem;


// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main() {
    gl_Position = projection * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform float offsetx;
uniform float offsety;

void main() {
    FragColor = texture(texture1, vec2(TexCoord.x + offsetx, TexCoord.y + offsety));
}
)";

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool isCoin = true, isCoin_2 = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Funções auxiliares

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);
std::vector<GLuint> loadTexturesFromFolder(const std::string& folderPath);
std::vector<std::vector<int>> loadMap(const std::string& filename, int width, int height);
void drawTilemap(const std::vector<std::vector<int>>& map, int width, int height, int tileSize, Sprite& sprite, const std::vector<GLuint>& textures);
GLuint loadTexture(std::string filePath, int &imgWidth, int &imgHeight);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
bool game(GLFWwindow* window, const std::vector<std::vector<int>>& map, Sprite& sprite, const std::vector<GLuint>& textures);
bool Collision(SpriteChar &one, SpriteChar &two);

int main() {

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Texture", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Definindo as dimensções da viewport com as mesmas dimensções da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

    //Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

    float vertices[] = {
    // positions          // texture coords
    -0.5f,  0.0f,  0.0f,  0.0f, 0.5f, // left
     0.0f,  0.25f, 0.0f,  0.5f, 1.0f, // top
     0.0f, -0.25f, 0.0f,  0.5f, 0.0f, // bottom
     0.5f,  0.0f,  0.0f,  1.0f, 0.5f  // right
    };

    unsigned int indices[] = {
    0, 1, 2, // first triangle
    2, 1, 3  // second triangle
    };


    // Buffer e array de vértices
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    std::vector<GLuint> texturesMap = loadTexturesFromFolder("textures");

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



    Sprite spriteMap(VAO, shaderProgram);
    spriteMap.setScale(57.0f, 57.0f);

    std::vector<std::vector<int>> map = loadMap("map.txt", 15, 15);





    int tecla = 10;
	while(tecla == 10)
	{
		if(game(window, map, spriteMap, texturesMap)){
			cout << "Você venceu" << endl;
		}
		else{
			cout << "Você perdeu" << endl;
		}

		cout << "Deseja jogar novamente? Aperte Enter, ou outra tecla para sair." << endl;
		tecla = cin.get();
	}






    return 0;
}


std::vector<std::vector<int>> loadMap(const std::string& filename, int width, int height) {
    std::vector<std::vector<int>> map(height, std::vector<int>(width));
    std::ifstream file(filename);
    if (file.is_open()) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                file >> map[y][x];
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
    return map;
}

std::vector<GLuint> loadTexturesFromFolder(const std::string& folderPath) {
    std::vector<GLuint> textureIDs;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".png") {
            GLuint textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            std::string pathStr = entry.path().string();
            const char* pathCStr = pathStr.c_str();
            unsigned char* data = stbi_load(pathCStr, &width, &height, &nrComponents, 0);
            if (data) {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
                textureIDs.push_back(textureID);
            } else {
                std::cerr << "Failed to load texture: " << entry.path() << std::endl;
                stbi_image_free(data);
            }
        }
    }
    return textureIDs;
}


GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLint success;
    GLchar infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Função para desenhar o mapa
void drawTilemap(const std::vector<std::vector<int>>& map, int width, int height, int tileSize, Sprite& sprite, const std::vector<GLuint>& textures) {
    float halfTileSize = tileSize / 2.0f;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int tileIndex = map[y][x];
            if (tileIndex >= 0 && tileIndex < textures.size()) {
                sprite.setTexture(textures[tileIndex]);

                // Cálculo das posições isométricas ajustadas
                float isoX = (x - y) * halfTileSize + 400.0f;
                float isoY = (x + y) * halfTileSize / 2.0f + 100.0f;

                sprite.setScale(tileSize, tileSize); // Ajuste de escala para isométrico
                sprite.setPosition(isoX, isoY); // Ajuste de posição central
                sprite.draw();
            }
        }
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

		//SpriteChar::processInput( window);
}

GLuint loadTexture(std::string filePath, int &imgWidth, int &imgHeight)
{
	GLuint texID;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
    	if (nrChannels == 3) //jpg, bmp
    	{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    	}
    	else //png
    	{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    	}
    	glGenerateMipmap(GL_TEXTURE_2D);

		imgWidth = width;
		imgHeight = height;

		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
    	 std::cout << "Failed to load texture" << std::endl;
	}


	return texID;
}

bool Collision(SpriteChar &one, SpriteChar &two)
{
	//TODO
}


glm::vec3 posOnMap(glm::vec2 iPos, glm::vec2 posIni, glm::vec2 tileSize)
{
	glm::vec3 pos;
	//Encontra a posição no mapa diamond
	pos.x = posIni.x + (iPos.x-iPos.y) * tileSize.x/2.0f;
	pos.y = posIni.y + (iPos.x+iPos.y) * tileSize.y/2.0f;
	return pos;
}

bool game(GLFWwindow* window, const std::vector<std::vector<int>>& map, Sprite& sprite, const std::vector<GLuint>& textures)
{

	//inicializa variaveis globais para o valor padrão
	isCoin = isCoin_2 = true;


	//Criação dos agentes

//	int imgWidth, imgHeight;
//  GLuint idleTexture = loadTexture("IdleAnim/idle.png", imgWidth, imgHeight);
//  GLuint walkTexture = loadTexture("WalkAnim/walk.png", imgWidth, imgHeight);
//  GLuint coinTexture = loadTexture("Coins/gold.png", imgWidth, imgHeight);
//  GLuint coinTexture_2 = loadTexture("Coins/gold.png", imgWidth, imgHeight);
//
//	playeriPos.x = 0; //coluna
//	playeriPos.y = 0; //linha
//	glm::vec3 playerPos = posOnMap(playeriPos, posIni, tileSize);
//
//  coiniPos.x = 5
//	coiniPos.y = 10;
//	glm::vec3 coinPos_2 = posOnMap(coiniPos, posIni, tileSize);
//
//  coiniPos.x = 2;
//	coiniPos.y = 4;
//	glm::vec3 coinPos = posOnMap(coiniPos, posIni, tileSize);
//
//	SpriteChar player coin, coin_2;
//	SpriteChar player(idleTexture, 4, 6, playerPos, glm::vec3(imgWidth*3,imgHeight*3,1.0),0.0,glm::vec3(1.0,0.0,1.0));
//  SpriteChar coin  (coinTexture, 4, 6, coinPos, glm::vec3(imgWidth*3,imgHeight*3,1.0),0.0,glm::vec3(1.0,0.0,1.0));
//  SpriteChar coin_2 (coinTexture_2, 4, 6, coinPos_2, glm::vec3(imgWidth*3,imgHeight*3,1.0),0.0,glm::vec3(1.0,0.0,1.0));
//



	// Render loop
	while (!glfwWindowShouldClose(window))
	{

		glfwPollEvents();

		// Atualizando input personagem
		// Verifica movimentação do personagem
//		if (!SpriteChar::directions::NONE)
//		{
//			player.setPosition(posOnMap(playeriPos, posIni, tileSize));
//			player.draw(true);
//			if (Collision(player, coin)){
//				coin.removeSprite();
//				isCoin = false;
//			}
//			else if (Collision(player, coin_2)){
//				coin2.removeSprite();
//				isCoin_2 = false;
//			}
//
//			if (tilemap[(int)round(playeriPos.y)][(int)round(playeriPos.x)] == 3)
//			{
//				glfwSetWindowShouldClose(window, GL_TRUE);
//				glfwTerminate();
//				return false;
//			}
//
//
//
//		}

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		drawTilemap(map, 15, 15, 57, sprite, textures);


		if (!isCoin && !isCoin_2){
			cout<<"Você venceu!"<<endl;
			break;
		}
		if (isCoin)
			//coin.draw(false);

		if (isCoin_2)
			//coin_2.draw(false);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}



    glfwDestroyWindow(window);
    glfwTerminate();

	return true;
}
