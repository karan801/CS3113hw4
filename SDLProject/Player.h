#include "Entity.h"

class Player : public Entity {
    
public:
    int attackCount = 0;
    
    Player() noexcept;
    
    bool CheckCollision(Entity *other) override;
    void CheckCollisionsY(Entity *objects, int objectCount) override;
    void CheckCollisionsX(Entity *objects, int objectCount) override;
    void CheckCollisionsY(Map *map) override;
    void CheckCollisionsX(Map *map) override;

    void Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map) override;
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index) override;
    void Render(ShaderProgram *program) override;
};
