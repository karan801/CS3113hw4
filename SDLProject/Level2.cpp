#include "Level2.h"

#define LEVEL2_WIDTH 28
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMY_COUNT 6

unsigned int level2_data[] =
{//0 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, //0
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //-1
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //-2
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //-3
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 3, //-4
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //-5
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, //-6
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3  //-7
};

void Level2::resetPosition() {
    state.player->position = glm::vec3(3.0f, -3.0f, 0.0f);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        if (i < 3)
            state.enemies[i].position = glm::vec3(5.5f+(1.0f*i),-4.0f,0.0f);
        else if (i == 4)
            state.enemies[i].position = glm::vec3(16.0f,-4.0f,0.0f);
        else if (i == 5)
            state.enemies[i].position = glm::vec3(21.0f,-3.0f,0.0f);
        else
            state.enemies[i].position = glm::vec3(25.0f,-6.0f,0.0f);
    }
}

void Level2::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Player();
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
    
    //state.player->jumpPower = 7.5f;
    state.player->type = PLAYER;

    // Initialize Enemies
    state.enemies = new Enemy[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        state.enemies[i].type = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        
        if (i < 3)
            state.enemies[i].position = glm::vec3(5.5f+(1.0f*i),-4.0f,0.0f);
        else if (i == 4)
            state.enemies[i].position = glm::vec3(16.0f,-4.0f,0.0f);
        else if (i == 5)
            state.enemies[i].position = glm::vec3(21.0f,-3.0f,0.0f);
        else
            state.enemies[i].position = glm::vec3(25.0f,-6.0f,0.0f);
        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 1.0f;
        state.enemies[i].acceleration = glm::vec3(0.0f, -4.0f, 0.0f);
        state.enemies[i].speed = 1;
    }
    // tiles
    
    state.map = new Map (LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
}
    
GameMode Level2::Update(float deltaTime) {
    if (state.player->health > 0) state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }
    if (state.player->position.y < -8.0f) {
        state.player->health -=1;
        resetPosition();
    }
    if (state.player->health <= 0) {
        state.nextScene = 0;
        return MENUF;
    }
    if (state.player->position.x >= 25.0f) {
        state.nextScene = 3;
    }
    return GOING;
}
    
void Level2::Render(ShaderProgram *program) {
    glClearColor(0.180f, 0.180f, 0.180f, 1.0f);
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++)
        state.enemies[i].Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program,fontTextureID,"Lives: "+std::to_string(state.player->health),0.3f,0.0f,glm::vec3(3.0f, -6.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"Level 2",0.3f,0.0f,glm::vec3(3.0f, -7.0f, 0.0f));
}
