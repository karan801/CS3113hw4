#include "Scene.h"
#include "Util.h"

class SnakePit : public Scene {
    
public:
    void Initialize() override;
    GameMode Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
