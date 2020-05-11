#include "Entity.h"

class Enemy : public Entity {
    

enum AIType {COBRA};
enum AIState {IDLE, ATTACKING, WALKING, DEAD};

public:
    AIType aiType;
    AIState aiState;
    
    Enemy() noexcept;
    
    //collisions
    bool CheckCollision(Entity *other) override;
    void CheckCollisionsY(Entity *objects, int objectCount) override;
    void CheckCollisionsX(Entity *objects, int objectCount) override;
    void CheckCollisionsY(Map *map) override;
    void CheckCollisionsX(Map *map) override;

    //game functions
    void Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map) override;
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index) override;
    void Render(ShaderProgram *program) override;
    
    //AI
    void AI(Entity *player, Entity *enemies, int enemyCount);
    void AISnake(Entity *player);
};
