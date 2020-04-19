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

enum EntityType {NONE, PLAYER, PLATFORM, ENEMY};
enum AIType {WALKER, JUMPER, STALKER, FOLLOWER};
enum AIState {IDLE, WALKING, JUMPING, STALKING, DEAD, FOLLOWING, LEADING};
enum GameMode {GOING,MENU,MENUW,MENUF};


class Entity {
public:
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    AIType aiType;
    AIState aiState;
    EntityType type;
    EntityType lastCollided;
    
    
    float width = 1;
    float height = 1; //may need to change these 2 variables as needed, otherwise fixed for now.
    
    bool jump = false;
    float jumpPower = 2.0f;
    int jumpCount = 0;
    
    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
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
    
    bool isActive = true;
    int lives = 3;
    bool invulnerability = false;
    int invulnerabilityCount = 0;
    
    bool collidedTop = false;
    bool collidedTopLeft = false;
    bool collidedTopRight = false;
    bool collidedBottom = false;
    bool collidedBottomLeft = false;
    bool collidedBottomRight = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    Entity();
    
    bool CheckCollision(Entity *other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);
    
    void Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    Entity findNearestFollower(Entity *enemies, int enemyCount);
    
    void AI(Entity *player, Entity *enemies, int enemyCount);
    void AIWalker();
    void AIJumper(Entity *player);
    void AIStalker(Entity *player);
    void AIFollower(Entity *player, Entity *enemies, int enemyCount);
};
