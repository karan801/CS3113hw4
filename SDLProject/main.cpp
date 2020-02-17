#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
ShaderProgram programz;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 zviewMatrix, zMatrix, zprojectionMatrix;

float player_x_rotate = 0;
float player_z = 0;

GLuint playerxTextureID;
GLuint playerzTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
if (image == NULL) {
std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
}
GLuint textureID;
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 1!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    glUseProgram(program.programID);
    playerxTextureID = LoadTexture("ctg.png");
    
    programz.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    zviewMatrix = glm::mat4(1.0f);
    zMatrix = glm::mat4(1.0f);
    zprojectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -1.0f, 1.0f);
    
    programz.SetProjectionMatrix(zprojectionMatrix);
    programz.SetViewMatrix(zviewMatrix);
    glUseProgram(programz.programID);
    playerzTextureID = LoadTexture("sword.png");
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    player_x_rotate += 180.0f * deltaTime;
    player_z += 1.0f * deltaTime;
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-player_x_rotate),glm::vec3(0.0f, 0.0f, 1.0f));
    
    zMatrix = glm::mat4(1.0f);
    zMatrix = glm::translate(zMatrix, glm::vec3(player_z,0.0f,0.0f));
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(modelMatrix);
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
     glEnableVertexAttribArray(program.positionAttribute);
     glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
     glEnableVertexAttribArray(program.texCoordAttribute);
    
     glBindTexture(GL_TEXTURE_2D, playerxTextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
     
     
     glDisableVertexAttribArray(program.positionAttribute);
     glDisableVertexAttribArray(program.texCoordAttribute);
    // z
    programz.SetModelMatrix(zMatrix);
    glVertexAttribPointer(programz.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
     glEnableVertexAttribArray(programz.positionAttribute);
     glVertexAttribPointer(programz.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
     glEnableVertexAttribArray(programz.texCoordAttribute);
    
     glBindTexture(GL_TEXTURE_2D, playerzTextureID);
     glDrawArrays(GL_TRIANGLES, 0, 6);
     
     
     glDisableVertexAttribArray(programz.positionAttribute);
     glDisableVertexAttribArray(programz.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow); }


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
