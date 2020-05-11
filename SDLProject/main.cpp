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

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Effects.h"
#include "Scene.h"
#include "Menu.h"
#include "Tent.h"
#include "SnakePit.h"

#include <vector>
#include <iostream>

#include <SDL_mixer.h>

using namespace std;

Scene *currentScene;
Scene *sceneList[3];
GameMode mode = MENU;
bool gameIsRunning = true;

SDL_Window* displayWindow;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint uifontTextureID;

Effects *effects;

Mix_Music *music;
Mix_Chunk *hiss;

void SwitchToScene(int sceneNum) {
    currentScene = sceneList[sceneNum];
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("The Snake Warrior!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("fretless.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    hiss = Mix_LoadWAV("Snake Strike 01.wav");
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/effects_textured.glsl");
    
    uifontTextureID = Util::LoadTexture("pixel_font.png");
    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
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
    sceneList[0] = new Menu();
    sceneList[1] = new Tent();
    sceneList[2] = new SnakePit();
    SwitchToScene(0);
    
    effects = new Effects(projectionMatrix, viewMatrix);
    //effects->Start(GROW, 10.0f);
}

void ProcessInput() {
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        if (mode == MENU || mode == MENUF || mode == MENUW) {
                            SwitchToScene(1);
                            mode = GOING;
                        }
                        break;
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_UP:
                        // alternative jump
                        //currentScene->state.player->jump = true;
                        break;
                        
                    case SDLK_a:
                        // Move the player left (alternative)
                        break;
                        
                    case SDLK_d:
                        // Move the player right (alternative)
                        break;
                        
                    case SDLK_w:
                        // alternative jump
                        //currentScene->state.player->jump = true;
                        break;
                        
                    case SDLK_SPACE:
                        //if (state.player->collidedBottom) {
                        //}
                        currentScene->state.player->attack = true;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        currentScene->state.player->movement.x += -1.5f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        currentScene->state.player->direction = false;
    }
    else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        currentScene->state.player->movement.x += 1.5f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
        currentScene->state.player->direction = true;
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        currentScene->state.player->movement.y += -1.5f;
        if (glm::length(currentScene->state.player->movement.x) == 0){ currentScene->state.player->animIndices = currentScene->state.player->animLeft;
            currentScene->state.player->direction = false;
        }
    }
    else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        currentScene->state.player->movement.y += 1.5f;
        if (glm::length(currentScene->state.player->movement.x) == 0){
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
            currentScene->state.player->direction = false;
        }
    }
    
    if (glm::length(currentScene->state.player->movement) > 1.5f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
    if (glm::length(currentScene->state.player->movement) == 0) {
        currentScene->state.player->animIndices = currentScene->state.player->animIdle;
        currentScene->state.player->animIndex = 0;
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

bool lastCollidedBottom = false;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        if (mode != MENU && mode != MENUF && mode != MENUW){
            mode = currentScene->Update(FIXED_TIMESTEP);
        }
        
        effects->Update(FIXED_TIMESTEP);
        if (mode == MENUF) {
            SwitchToScene(0);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    
    viewMatrix = glm::mat4(1.0f);
    if (currentScene == sceneList[0]) {
        viewMatrix = glm::translate(viewMatrix,glm::vec3(-2.0f,2.0f,0));
    }
    if (currentScene == sceneList[1]) { //for tent base level
        if (currentScene->state.player->position.x > 5.0f && currentScene->state.player->position.x < 10.0f) //normal view
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-currentScene->state.player->position.x, 0, 0));
        else if (currentScene->state.player->position.x < 5.0f) //left view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-5.0f, 0, 0));
        else if (currentScene->state.player->position.x > 10.0f)//right view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-10.0f, 0, 0));
        if (currentScene->state.player->position.y > -7.0f && currentScene->state.player->position.y < -4.0f) //normal view
            viewMatrix = glm::translate(viewMatrix,glm::vec3(0, -currentScene->state.player->position.y, 0));
        else if (currentScene->state.player->position.y < -4.0f) //top view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(0, 7.0f, 0));
        else if (currentScene->state.player->position.y > -7.0f)//bottom view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(0, 4.0f, 0));
    } else if (currentScene == sceneList[2]) { //for snakepit level
        if (currentScene->state.player->position.x > 5.0f && currentScene->state.player->position.x < 10.0f) //normal view
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-currentScene->state.player->position.x, 0, 0));
        else if (currentScene->state.player->position.x < 5.0f) //left view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-5.0f, 0, 0));
        else if (currentScene->state.player->position.x > 10.0f)//right view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-10.0f, 0, 0));
        if (currentScene->state.player->position.y > -13.0f && currentScene->state.player->position.y < -4.0f) //normal view
            viewMatrix = glm::translate(viewMatrix,glm::vec3(0, -currentScene->state.player->position.y, 0));
        else if (currentScene->state.player->position.y < -4.0f) //top view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(0, 13.0f, 0));
        else if (currentScene->state.player->position.y > -13.0f)//bottom view wall
            viewMatrix = glm::translate(viewMatrix,glm::vec3(0, 4.0f, 0));
    }
    viewMatrix = glm::translate(viewMatrix,effects->viewOffset);
}


void Render() {
    program.SetViewMatrix(viewMatrix);
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    currentScene->Render(&program);
    effects->Render();
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");
    
    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    Util::DrawText(&program, fontTextureID, ("Lives: "+ to_string(currentScene->state.player->health)), 0.1f,0.1f, glm::vec3(-6, 3.2, 0));
    if (currentScene == sceneList[1])
        Util::DrawText(&program,fontTextureID,"Campsite",0.1f,0.1f,glm::vec3(-6, 2.7, 0));
    if (currentScene == sceneList[2]) {
        Util::DrawText(&program,fontTextureID,"The Snake Pit",0.1f,0.1f,glm::vec3(-6, 2.7, 0));
        int enemiesleft = 0;
        for (int i = 0; i < sizeof(currentScene->state.enemies); i++)
            if (currentScene->state.enemies[i].isActive)
                enemiesleft++;
        Util::DrawText(&program,fontTextureID,("Enemies: " + to_string(enemiesleft)),0.1f,0.1f,glm::vec3(-6, 2.2, 0));
    }
    //for (int i = 0; i < 3; i++) {
        //Util::DrawIcon(&program, heartTextureID, glm::vec3(5+(i*0.5f), 3.2, 0));
    //}
    
    if (mode == MENUW) {
        Util::DrawText(&program,fontTextureID,"Congrats! Thanks for playing professor!",0.1f,0.1f,glm::vec3(-4.0f, 1.0f, 0.0f));
        Util::DrawText(&program,fontTextureID,"I really hope to build more games in the future",0.1f,0.1f,glm::vec3(-4.0f, 0.5f, 0.0f));
        Util::DrawText(&program,fontTextureID,"(preferably without the stress of other finals).",0.1f,0.1f,glm::vec3(-4.0f, 0.0f, 0.0f));
    }
    if (mode == MENUF)
        Util::DrawText(&program,fontTextureID,"Failure. You lost. Play again?",0.1f,0.1f,glm::vec3(-4.0f, 0.0f, 0.0f));
    
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
        
        if (currentScene->state.nextScene == 3) {
            mode = MENUW;
            SwitchToScene(0);
        } else if (currentScene == sceneList[2] && currentScene->state.nextScene == 1){
            SwitchToScene(currentScene->state.nextScene);
            currentScene->state.player->position = glm::vec3(4.0f, -8.0f, 0.0f);
        }
        else if (currentScene->state.nextScene > 0) {
            SwitchToScene(currentScene->state.nextScene);
        }
        Render();
    }
    
    Shutdown();
    return 0;
}
