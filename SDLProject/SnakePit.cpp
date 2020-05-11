#include "SnakePit.h"

#define SNAKEPIT_WIDTH 16
#define SNAKEPIT_HEIGHT 18
#define SNAKEPIT_ENEMY_COUNT 6

unsigned int snakepit_data[] =
{// 0    1    2   3   4   5   6   7   8   9   10  11  12  13   14   15
    141, 141, 141,141,141,141,141,141,141,141,141,141,141,141, 141, 141,  // 0
    141, 141, 141,21, 21, 141,141,141,141,141,141,141,141,141, 141, 141,  //-1
    141, 123, 161,163,163,161,161,161,161,161,161,161,161,161, 124, 141, //-2
    141, 142, 21, 3,  5,  21, 21, 21, 21, 21, 21, 21, 21, 21,  140, 141, //-3
    141, 142, 21, 23, 25, 21, 21, 21, 21, 21, 21, 21, 21, 21,  140, 141, //-4
    141, 142, 21, 23, 40, 4,  4,  4,  5,  21, 21, 21, 21, 21,  140, 141, //-5
    141, 142, 21, 23, 0,  44, 44, 44, 45, 21, 21, 21, 21, 21,  140, 141, //-6
    141, 142, 21, 23, 25, 21, 21, 21, 21, 21, 21, 21, 21, 21,  140, 141, //-7
    141, 142, 21, 23, 25, 21, 21, 21, 21, 21, 21, 21, 21, 21,  140, 141, //-8
    141, 142, 21, 23, 25, 21, 21, 21, 21, 21, 21, 21, 21, 21,  140, 141, //-9
    141, 142, 21, 43, 45, 21, 21, 21, 21, 3,  4,  4,  4,  5,   140, 141, //-10
    141, 142, 21, 21, 21, 21, 21, 21, 3,  42, 24, 24, 24, 25,  140, 141, //-11
    141, 142, 21, 21, 21, 21, 21, 21, 23, 24, 24, 24, 24, 25,  140, 141, //-12
    141, 142, 21, 21, 21, 21, 21, 3,  42, 24, 24, 24, 24, 25,  140, 141, //-13
    141, 142, 21, 21, 21, 21, 21, 43, 44, 44, 44, 44, 44, 45,  140, 141, //-14
    141, 143, 121,121,121,121,121,121,121,121,121,121,121,121, 144, 141, //-15
    141, 141, 141,141,141,141,141,141,141,141,141,141,141,141, 141, 141,  //-16
    141, 141, 141,141,141,141,141,141,141,141,141,141,141,141, 141, 141   //-17
}; //list of nonsolid blocks: 0-59, 163 on "Forest Tileset.png"

void SnakePit::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("Forest Tileset.png");
    state.map = new Map(SNAKEPIT_WIDTH, SNAKEPIT_HEIGHT, snakepit_data, mapTextureID, 1.0f, 20, 17);
    
    state.player = new Player();
    
    state.enemies = new Enemy[SNAKEPIT_ENEMY_COUNT];
    state.enemies[0].position = glm::vec3(7.0f,-14.0f,0.0f);
    state.enemies[1].position = glm::vec3(8.0f,-13.0f,0.0f);
    state.enemies[2].position = glm::vec3(12.0f,-4.0f,0.0f);
    state.enemies[3].position = glm::vec3(10.0f,-9.0f,0.0f);
    state.enemies[4].position = glm::vec3(13.0f,-13.0f,0.0f);
    state.enemies[5].position = glm::vec3(12.0f,-13.0f,0.0f);
}
    
GameMode SnakePit::Update(float deltaTime) {
    if (state.player->health > 0)
        state.player->Update(deltaTime, state.player, state.enemies, SNAKEPIT_ENEMY_COUNT, state.map);
    int allDead = 0;
    for (int i = 0; i < SNAKEPIT_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, SNAKEPIT_ENEMY_COUNT, state.map);
        if (!state.enemies[i].isActive) //for every dead enemy, update allDead
            allDead++;
    }
    
    if (state.player->health <= 0) {
        state.nextScene = 0;
        return MENUF;
    }
    if (state.player->position.y > -2) { //return to campsite!
        state.nextScene = 1;
    }
    if (allDead == SNAKEPIT_ENEMY_COUNT) { //if all enemies calculated to be dead, then the game has been won
        state.nextScene = 3;
        return MENUW;
    }
    return GOING;
}
    
void SnakePit::Render(ShaderProgram *program) {
    glClearColor(0.180f, 0.180f, 0.180f, 1.0f);
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < SNAKEPIT_ENEMY_COUNT; i++)
        state.enemies[i].Render(program);
}
