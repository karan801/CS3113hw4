#pragma once
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

#include <SDL_mixer.h>

#include <time.h>
#include "Map.h"
#include "Util.h"

enum EntityType {AIR, PLAYER, PLATFORM, ENEMY, NPC};
enum GameMode {GOING,MENU,MENUW,MENUF};

class Entity {
public:
    //==VARIABLES==
    //movement
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed;
    
    glm::mat4 modelMatrix;
    
    //active/life
    EntityType type;
    bool isActive = true;
    int health = 0;
    bool invulnerability = false;
    int invulnerabilityCount = 0;
    
    EntityType lastCollided;
    bool collidedTop = false;
    bool collidedTopLeft = false;
    bool collidedTopRight = false;
    bool collidedBottom = false;
    bool collidedBottomLeft = false;
    bool collidedBottomRight = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    float width = 1;
    float height = 1; //may need to change these 2 variables as needed, otherwise fixed for now.
    
    //texturing
    GLuint textureID;
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
    
    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    //==METHODS==
    //collision functions
    virtual bool CheckCollision(Entity *other) = 0;
    virtual void CheckCollisionsY(Entity *objects, int objectCount) = 0;
    virtual void CheckCollisionsX(Entity *objects, int objectCount) = 0;
    virtual void CheckCollisionsX(Map *map) = 0;
    virtual void CheckCollisionsY(Map *map) = 0;
    
    //game functions
    virtual void Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map) = 0;
    virtual void Render(ShaderProgram *program) = 0;
    virtual void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index) = 0;

};
