#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#include <vector>
#include <iostream>

#define PLATFORM_COUNT 30

#define SOLUTION_COUNT 2
using namespace std;

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *solution;
    GameMode flag;
};

GameState state;


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    }
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
 

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Karan's Lunar Lander!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0, 4.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.1f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("lunar_lander.png");/*
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;*/
    
    state.player->height = 1.0f; // to help with graphic empty edges
    state.player->width = 1.0f;
    
    state.player->jumpPower = 0.3f;
    state.player->type = PLAYER;
    
    //solution tiles
    state.solution = new Entity[SOLUTION_COUNT];
    for (int i = 0; i < SOLUTION; i++){
        state.solution[i].type = SOLUTION;
    }
    GLuint solutionTextureID = LoadTexture("solution_tile.png");
    state.solution[0].textureID = solutionTextureID;
    state.solution[0].position = glm::vec3(2.0f, -3.5f, 0.0f);
    state.solution[1].textureID = solutionTextureID;
    state.solution[1].position = glm::vec3(3.0f, -3.5f, 0.0f);
    for (int i = 0; i < SOLUTION_COUNT; i++) {
        state.solution[i].Update(0, NULL, 0);
    }
    
    
    //spike tiles
    state.platforms = new Entity[PLATFORM_COUNT];
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].type = PLATFORM;
    }
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");
    
    //floor - while I realized after that I should have done this more efficently, I have stuck to this for now
    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].position = glm::vec3(-3.0f, -3.5f, 0.0f);
    
    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].position = glm::vec3(-2.0f, -3.5f, 0.0f);
    
    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].position = glm::vec3(-1.0f, -3.5f, 0.0f);
    
    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].position = glm::vec3(0.0f, -3.5f, 0.0f);
    
    state.platforms[4].textureID = platformTextureID;
    state.platforms[4].position = glm::vec3(1.0f, -3.5f, 0.0f);
    
    state.platforms[5].textureID = platformTextureID;
    state.platforms[5].position = glm::vec3(4.0f, -3.5f, 0.0f);
    
    state.platforms[6].textureID = platformTextureID;
    state.platforms[6].position = glm::vec3(-4.0f, -3.5f, 0.0f);
    
    //right wall
    state.platforms[7].textureID = platformTextureID;
    state.platforms[7].position = glm::vec3(4.0f, -2.5f, 0.0f);
    
    state.platforms[8].textureID = platformTextureID;
    state.platforms[8].position = glm::vec3(4.0f, -1.5f, 0.0f);
    
    state.platforms[9].textureID = platformTextureID;
    state.platforms[9].position = glm::vec3(4.0f, -0.5f, 0.0f);
    
    state.platforms[10].textureID = platformTextureID;
    state.platforms[10].position = glm::vec3(4.0f, 1.5f, 0.0f);
    
    state.platforms[11].textureID = platformTextureID;
    state.platforms[11].position = glm::vec3(4.0f, 2.5f, 0.0f);
    
    state.platforms[12].textureID = platformTextureID;
    state.platforms[12].position = glm::vec3(4.0f, 3.5f, 0.0f);
    
    state.platforms[13].textureID = platformTextureID;
    state.platforms[13].position = glm::vec3(4.0f, 0.5f, 0.0f);
    
    //left wall
    state.platforms[14].textureID = platformTextureID;
    state.platforms[14].position = glm::vec3(-4.0f, -2.5f, 0.0f);
    
    state.platforms[15].textureID = platformTextureID;
    state.platforms[15].position = glm::vec3(-4.0f, -1.5f, 0.0f);
    
    state.platforms[16].textureID = platformTextureID;
    state.platforms[16].position = glm::vec3(-4.0f, -0.5f, 0.0f);
    
    state.platforms[17].textureID = platformTextureID;
    state.platforms[17].position = glm::vec3(-4.0f, 0.5f, 0.0f);
    
    state.platforms[18].textureID = platformTextureID;
    state.platforms[18].position = glm::vec3(-4.0f, 1.5f, 0.0f);
    
    state.platforms[19].textureID = platformTextureID;
    state.platforms[19].position = glm::vec3(-4.0f, 2.5f, 0.0f);
    
    state.platforms[20].textureID = platformTextureID;
    state.platforms[20].position = glm::vec3(-4.0f, 3.5f, 0.0f);
    
    //obstacles
    //platform 1 (towards bottom middle)
    state.platforms[21].textureID = platformTextureID;
    state.platforms[21].position = glm::vec3(1.0f, -1.25f, 0.0f);
    state.platforms[22].textureID = platformTextureID;
    state.platforms[22].position = glm::vec3(-1.0f, -1.25f, 0.0f);
    state.platforms[23].textureID = platformTextureID;
    state.platforms[23].position = glm::vec3(0.0f, -1.25f, 0.0f);
    //platform 2 (towards right middle)
    state.platforms[24].textureID = platformTextureID;
    state.platforms[24].position = glm::vec3(1.25f, 1.75f, 0.0f);
    state.platforms[25].textureID = platformTextureID;
    state.platforms[25].position = glm::vec3(2.0f, 1.75f, 0.0f);
    state.platforms[26].textureID = platformTextureID;
    state.platforms[26].position = glm::vec3(3.0f, 1.75f, 0.0f);
    //platform 3 (towards left top)
    state.platforms[27].textureID = platformTextureID;
    state.platforms[27].position = glm::vec3(-2.0f, 1.75f, 0.0f);
    state.platforms[28].textureID = platformTextureID;
    state.platforms[28].position = glm::vec3(-3.0f, 1.75f, 0.0f);
    state.platforms[29].textureID = platformTextureID;
    state.platforms[29].position = glm::vec3(-1.25f, 1.75f, 0.0f);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
    }
 
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_UP:
                        // alternative jump
                        state.player->jump = true;
                        break;
                        
                    case SDLK_a:
                        // Move the player left (alternative)
                        break;
                        
                    case SDLK_d:
                        // Move the player right (alternative)
                        break;
                        
                    case SDLK_w:
                        // alternative jump
                        state.player->jump = true;
                        break;
                        
                    case SDLK_SPACE:
                        //if (state.player->collidedBottom) {
                        //}
                        state.player->jump = true;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->acceleration.x += -3.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->acceleration.x += 3.0f;
        state.player->animIndices = state.player->animRight;
    }
    if (keys[SDL_SCANCODE_A]) {
        state.player->acceleration.x += -3.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->acceleration.x += 3.0f;
        state.player->animIndices = state.player->animRight;
    }

    if (glm::length(state.player->movement) > 20.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    if (state.flag == GOING) {
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;
        
        deltaTime += accumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }
        
        while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
            state.flag = state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT); //here are the list of platforms and the number of platforms, look at each platform and check if im colliding. get out. dont change anything/update
            
            state.flag = state.player->Update(FIXED_TIMESTEP, state.solution, SOLUTION_COUNT);
            
            deltaTime -= FIXED_TIMESTEP;
        }
        
        accumulator = deltaTime;
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    for (int i = 0; i < SOLUTION_COUNT; i++) {
        state.solution[i].Render(&program);
    }
    state.player->Render(&program);
    GLuint fontTextureID = LoadTexture("font1.png");
    DrawText(&program,fontTextureID,"Reach the gray boxes!",0.3f,0.0f,glm::vec3(-4.25f, 3.5f, 0.0f));
    DrawText(&program,fontTextureID,"Don't touch the ground.",0.3f,0.0f,glm::vec3(-4.25f, 3.0f, 0.0f));
    if (state.flag == FAILURE) {
        DrawText(&program,fontTextureID,"FAILURE.",1.0f,0.0f,glm::vec3(-3.5f, 0.0f, 0.0f));
        state.player->isActive = false;
    } else if (state.flag == SUCCESS) {
        DrawText(&program,fontTextureID,"SUCCESS!",1.0f,0.0f,glm::vec3(-3.5f, 0.0f, 0.0f));
        state.player->isActive = false;
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


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
