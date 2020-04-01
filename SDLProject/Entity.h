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

#include <time.h>

enum EntityType {NONE, PLAYER, PLATFORM, ENEMY};
enum AIType {WALKER, WAITANDGO, JUMPER, STALKER};
enum AIState {IDLE, WALKING, JUMPING, STALKING, DEAD};
enum GameMode {GOING,SUCCESS,FAILURE};


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
    
    GameMode flag = GOING;
    
    float width = 1;
    float height = 1; //may need to change these 2 variables as needed, otherwise fixed for now.
    
    bool jump = false;
    float jumpPower = 1.0f;
    
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
    
    EntityType collidedTop = NONE;
    EntityType collidedBottom = NONE;
    EntityType collidedLeft = NONE;
    EntityType collidedRight = NONE;
    Entity();
    
    void flipActive();
    
    bool CheckCollision(Entity *other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
    
    GameMode Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Entity *platforms, int platformCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player, float deltaTime);
    void AIWalker();
    void AIWaitAndGo(Entity *player);
    void AIJumper(Entity *player);
    void AIStalker(Entity *player);
};
