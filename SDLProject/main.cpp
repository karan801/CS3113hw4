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

#include <SDL_mixer.h>

#define PLATFORM_COUNT 47

#define ENEMY_COUNT 3
using namespace std;

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
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

int jumpCount;
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Karan's Test with AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    state.player->position = glm::vec3(-4.0f, 2.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -4.0f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("george_0.png");
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f; // to help with graphic empty edges
    state.player->width = 0.75f;
    
    state.player->jumpPower = 4.0f;
    state.player->type = PLAYER;
    
    // Initialize Enemies
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("ctg.png");
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].type = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        if (i == 2)
            state.enemies[i].position = glm::vec3(3.0f,1.5f,0.0f);
        else if (i == 1)
            state.enemies[i].position = glm::vec3(-3.0,-2,0.0f);
        else
            state.enemies[i].position = glm::vec3(3.0f,-2.5f,0.0f);
        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 1.0f;
        state.enemies[i].acceleration = glm::vec3(0.0f, -4.0f, 0.0f);
        state.enemies[i].speed = 1;
    }
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].aiState = IDLE;
    jumpCount = 0;
    
    state.enemies[1].aiType = STALKER;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[2].aiType = WAITANDGO;
    state.enemies[2].aiState = IDLE;
    
    // tiles
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].type = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        if (i < 12) {
            state.platforms[i].position = glm::vec3(-5.0f + i, -3.5f, 0.0f);//floor
        }
        else if (i < 18) {
            state.platforms[i].position = glm::vec3(-5.0f, -2.5 + i-12, 0.0f); //left wall
        }
        else if (i < 27) {
            state.platforms[i].position = glm::vec3(5.0f, -2.5 + i-19, 0.0f); //right wall
        }
        else if (i < 39) {
            state.platforms[i].position = glm::vec3(-5.0f +i-27, 3.5f, 0.0f); //ceiling
        }
        else if (i < 44) {
            state.platforms[i].position = glm::vec3(0.0f +i-39, 0.0f, 0.0f); //middle platform
        }
        else if (i < 45) {
            state.platforms[i].position = glm::vec3(0.0f,1.0f,0.0f);
        }
        else if (i < 46) {
            state.platforms[i].position = glm::vec3(0.0f, -3.0f, 0.0f);
        }
        else {
            state.platforms[i].position = glm::vec3(-4.0f,0.0f,0.0f); //starting platform
        }
    }
    
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, NULL, NULL, NULL, 0);
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
        state.player->movement.x += -1.5f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x += 1.5f;
        state.player->animIndices = state.player->animRight;
    }
    if (keys[SDL_SCANCODE_A]) {
        state.player->movement.x += -1.5f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->movement.x += 1.5f;
        state.player->animIndices = state.player->animRight;
    }

    if (glm::length(state.player->movement) > 1.5f) {
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
        
        jumpCount++;
        while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
            state.player->Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.platforms, PLATFORM_COUNT); //here are the list of platforms and the number of platforms, look at each platform and check if im colliding. get out. dont change anything/update
            for (int i = 0; i < ENEMY_COUNT; i++){
                if (jumpCount/40 > 1 && state.enemies[i].aiType == JUMPER) {
                    state.enemies[i].jump = true;
                    jumpCount = 0;
                }
                state.flag = state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.platforms, PLATFORM_COUNT);
                if (state.flag == SUCCESS || state.flag == FAILURE)
                    break;
            }
            //state.flag = state.player->Update(FIXED_TIMESTEP, state.solution, SOLUTION_COUNT);
            
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
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }
    state.player->Render(&program);
    GLuint fontTextureID = LoadTexture("font1.png");
    DrawText(&program,fontTextureID,"Jump on their heads to win!",0.3f,0.0f,glm::vec3(-4.25f, 3.5f, 0.0f));
    if (state.flag == FAILURE) {
        DrawText(&program,fontTextureID,"FAILURE.",1.0f,0.0f,glm::vec3(-3.5f, 0.0f, 0.0f));
    } else if (state.flag == SUCCESS) {
        DrawText(&program,fontTextureID,"SUCCESS!",1.0f,0.0f,glm::vec3(-3.5f, 0.0f, 0.0f));
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
