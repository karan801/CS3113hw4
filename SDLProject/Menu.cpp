#include "Menu.h"

/*struct GameState {
    Map *map;
    Entity *player;
    Entity *enemies;
    int nextScene;
};*/

void Menu::Initialize() {
    state.nextScene = -1;
    state.player = new Player();
    
}
    
GameMode Menu::Update(float deltaTime) {
    state.nextScene = 1;
    state.player->position = glm::vec3(0,0,0);
    state.player->animIndices = state.player->animIdle;
    return MENU;
}
    
void Menu::Render(ShaderProgram *program) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");
    
    state.player->Render(program);
    Util::DrawText(program,fontTextureID,"The Snake Warrior!",0.1f,0.1f,glm::vec3(0.85f, -3.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"Press 'Enter' to start.",0.1f,0.1f,glm::vec3(0.75f, -4.0f, 0.0f));
}
