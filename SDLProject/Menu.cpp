#include "Menu.h"

#define MENU_WIDTH 5
#define MENU_HEIGHT 5

unsigned int menu_data[] =
{// 0   1   2   3   4   5   6
    3,  4,  4,  4,  5,  // 0
    23, 24, 24, 24, 25, //-1
    23, 24, 24, 24, 25, //-2
    23, 24, 24, 24, 25, //-3
    43, 44, 44, 44, 45, //-4
}; //list of nonsolid blocks: 0-59, 163 on "Forest Tileset.png"
/*struct GameState {
    Map *map;
    Entity *player;
    Entity *enemies;
    int nextScene;
};*/

void Menu::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("Forest Tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 20, 17);
    state.player = new Player();
    state.enemies = new Enemy();
    
}
    
GameMode Menu::Update(float deltaTime) {
    state.nextScene = 1;
    state.player->Update(deltaTime, state.player, NULL, 0, NULL);
    state.player->position = glm::vec3(0.0f,0.0f,0);
    return MENU;
}
    
void Menu::Render(ShaderProgram *program) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");
    state.map->Render(program);
    state.player->Render(program);
    Util::DrawText(program,fontTextureID,"The Snake Warrior!",0.3f,0.2f,glm::vec3(-2.0f, -5.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"'Enter' to start.",0.1f,0.1f,glm::vec3(0.0f, -3.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"'Space' to attack.",0.1f,0.1f,glm::vec3(0.0f, -3.5f, 0.0f));
}
