#include "Scene.h"
#include "Util.h"

class Level2 : public Scene {
    
public:
    void resetPosition();
    void Initialize() override;
    GameMode Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
