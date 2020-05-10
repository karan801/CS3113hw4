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
    state.player = new Player();
    return MENU;
}
    
void Menu::Render(ShaderProgram *program) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program,fontTextureID,"Karan's (Kind-of) First Game!",0.3f,0.0f,glm::vec3(0.85f, -3.0f, 0.0f));
    Util::DrawText(program,fontTextureID,"Press 'Enter' to start.",0.4f,0.0f,glm::vec3(0.75f, -4.0f, 0.0f));
}
