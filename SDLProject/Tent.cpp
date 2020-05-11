#include "Tent.h"

#define TENT_WIDTH 16
#define TENT_HEIGHT 12

unsigned int tent_data[] =
{// 0   1   2   3   4   5   6   7   8   9   10  11  12   13   14  15
    141, 141,141,141,141,141,141,141,141,141,141,141,141,141, 141, 141,  // 0
    141, 141,141,141,141,141,141,141,141,141,141,21,141,141, 141, 141,  //-1
    141, 123,161,161,161,161,161,161,161,161,161,163,161,161, 124, 141, //-2
    141, 142, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 140, 141, //-3
    141, 142, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 140, 141, //-4
    141, 142, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 140, 141, //-5
    141, 142, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 140, 141, //-6
    141, 142, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 140, 141, //-7
    141, 142, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 140, 141, //-8
    141, 143 ,121,121,121,121,121,121,121,121,121,121,121,121,144, 141, //-9
    141, 141,141,141,141,141,141,141,141,141,141,141,141, 141, 141,141,  //-10
    141, 141,141,141,141,141,141,141,141,141,141,141,141, 141, 141,141   //-11
}; //list of nonsolid blocks: 0-59, 163 on "Forest Tileset.png"

void Tent::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("Forest Tileset.png");
    state.map = new Map(TENT_WIDTH, TENT_HEIGHT, tent_data, mapTextureID, 1.0f, 20, 17);
    state.player = new Player();
}
    
GameMode Tent::Update(float deltaTime) {
    if (state.player->health > 0)
        state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
    if (state.player->position.y >= -2.0f) {
        state.nextScene = 2;
    }
    if (state.player->health <= 0) {
        state.nextScene = 0;
        return MENUF;
    }
    return GOING;
}
    
void Tent::Render(ShaderProgram *program) {
    glClearColor(0.275f, 0.510f, 0.706f, 1.0f);
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");
    Util::DrawText(program,fontTextureID,"Tent",0.1f,0.1f,glm::vec3(3.0f, -7.0f, 0.0f));
}
