#include "Level1.h"

#define LEVEL1_WIDTH 28
#define LEVEL1_HEIGHT 8
#define LEVEL1_ENEMY_COUNT 3

unsigned int level1_data[] =
{//0 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //0
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //-1
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, //-2
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 3, //-3
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0, 0, 0, 0, 0, 3, //-4
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 0, 3, //-5
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 0, 0, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 3, //-6
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 1, 1, 1, 3, 3, 3  //-7
};

void Level1::resetPosition() {
    state.player->position = glm::vec3(3.0f, -3.0f, 0.0f);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        if (i == 0)
            state.enemies[i].position = glm::vec3(13.0f,-4.0f,0.0f);
        //else if (i == 1)
            //state.enemies[i].position = glm::vec3(14.0f,-4.0f,0.0f);
        else
            state.enemies[i].position = glm::vec3(24.0f,-6.0f,0.0f);
    }
}

void Level1::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(3.0f, -3.0f, 0.0f);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -15.0f, 0.0f);
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
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
    
    state.player->jumpPower = 7.5f;
    state.player->type = PLAYER;
    
    // Initialize Enemies
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].type = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        if (i == 0)
            state.enemies[i].position = glm::vec3(13.0f,-4.0f,0.0f);
        else if (i == 1)
            state.enemies[i].position = glm::vec3(22.0f,-1.0f,0.0f);
        else
            state.enemies[i].position = glm::vec3(24.0f,-6.0f,0.0f);
        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 1.0f;
        state.enemies[i].acceleration = glm::vec3(0.0f, -4.0f, 0.0f);
        state.enemies[i].speed = 1;
    }
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = IDLE;
    
    state.enemies[1].aiType = JUMPER;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[2].aiType = STALKER;
    state.enemies[2].aiState = IDLE;
    
    // tiles
    state.map = new Map (LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
}
    
GameMode Level1::Update(float deltaTime) {
    if (state.player->lives > 0) state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
    if (state.player->position.y < -8.0f) {
        state.player->lives -= 1;
        resetPosition();
    }
    if (state.player->position.x >= 25.0f) {
        state.nextScene = 2;
    }
    if (state.player->lives <= 0) {
        state.nextScene = 0;
        return MENUF;
    }
    return GOING;
}
    
void Level1::Render(ShaderProgram *program) {
    glClearColor(0.275f, 0.510f, 0.706f, 1.0f);
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++)
        state.enemies[i].Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program,fontTextureID,"Level 1",0.3f,0.0f,glm::vec3(3.0f, -7.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"Lives: "+std::to_string(state.player->lives),0.3f,0.0f,glm::vec3(3.0f, -6.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"Reach the end to advance.",0.2f,0.0f,glm::vec3(1.1f, -0.5f, 0.0f));
}
