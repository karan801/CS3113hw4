#include "Scene.h"
#include "Util.h"

class Forest : public Scene {
    
public:
    void Initialize() override;
    GameMode Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
