#include "Player.h"

Player::Player() noexcept
{
    position = glm::vec3(3.0f, -3.0f, 0.0f); //coords
    movement = glm::vec3(0); //direction of moving
    acceleration = glm::vec3(0); //how fast speed increases
    velocity = glm::vec3(0); //how fast the entity is going
    speed = 5.0f; // a scalar of how fast the entity is going
    
    health = 3;
    modelMatrix = glm::mat4(1.0f);
    
    type = PLAYER;
    
    textureID = Util::LoadTexture("maleadventurer.png");
    animIdle = new int[13] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    animRight = new int[8] {13, 14, 15, 16, 17, 18, 19, 20};
    animLeft = new int[8] {117, 118, 119, 120, 121, 122, 123, 124};
    animAttackRight = new int[8] {27, 28, 29, 30, 31, 32, 33, 34};
    animAttackLeft = new int[8] {132, 133,134,135,136,137,138,139};
    animDeathRight = new int[2] {97, 98};
    animDeathLeft = new int[2] {202, 203};

    animIndices = animDown;
    animFrames = 4;
    animIndex = 3;
    animTime = 0;
    animCols = 13;
    animRows = 16;
    height = 0.4f;
    width = 0.35f;
    //int *animIndices = list of indices of sprites in texture atlas (animRight/left/up/down for different directions/movements)
    //int animFrames = number of frames
    //int animIndex = index of what we want to draw
    //animtime adds deltatime, then after some time passes, it adds the next index and then sets it back to 0 when we've exceeded frames
    //changing animtime can make things so much quicker and produce different effects (e.g. walking vs running)


}

bool Player::CheckCollision(Entity *other) {
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs (position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs (position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0){
        lastCollided = other->type;
        if (velocity.y > 0 || other->velocity.y < 0) {
            collidedTop = true;
        } else if (velocity.y < 0) {
            collidedBottom = true;
        }
        if (velocity.x > 0) {
            collidedRight = true;
        } else if (velocity.x < 0) {
            collidedLeft = true;
        }
        return true;
    }
    return false;
}
 
void Player::CheckCollisionsY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Player::CheckCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            } else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Player::CheckCollisionsY(Map *map) {
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTopLeft = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTopRight = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottomLeft = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottomRight = true;
    }
}

void Player::CheckCollisionsX(Map *map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Player::Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map)
{
    
    if (type == PLAYER && invulnerability) {
        invulnerabilityCount++;
        if (invulnerabilityCount > 300) {
            invulnerability = false;
            invulnerabilityCount = 0;
        }
    }
    collidedTop = false;
    collidedTopLeft = false;
    collidedTopRight = false;
    collidedBottom = false;
    collidedBottomLeft = false;
    collidedBottomRight = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (animIndices != NULL) {
        animFrames = sizeof(animIndices);
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            //animIndices = animIdle;
            animIndex = 0;
        }
    } else {
        animIndices = animIdle;
        animIndex = 0;
    }
    /*
    if (jump) { //instant VELOCTIY!
        jump = false;
        velocity.y += jumpPower;
        if(type == PLAYER) {
            Mix_VolumeMusic(MIX_MAX_VOLUME/4);
            Mix_Chunk *bounce = Mix_LoadWAV("Bounce.wav");
            Mix_PlayChannel(-1, bounce, 0);
        }
    }*/
    
    velocity.x = movement.x * speed; //if the player is tryna move, let them move
    velocity.y = movement.y * speed;
    velocity += acceleration * deltaTime; //changes velocity in relation to time
    
    position.y += velocity.y * deltaTime; // Move on Y
    
    position.x += velocity.x * deltaTime; // Move on X
   
    CheckCollisionsY(map);
    CheckCollisionsX(map);
    
    //CheckCollisionsY(enemies,enemyCount);
    //CheckCollisionsX(enemies,enemyCount); //deletes enemies??
    
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    //return flag;
}

void Player::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    //float texCoords[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Player::Render(ShaderProgram *program) {
    if (isActive == false) return;
    if (type == PLAYER && invulnerability && invulnerabilityCount%2)
        program->SetModelMatrix(modelMatrix);
    else if (type == PLAYER && invulnerability)
        return;
    else
        program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    } else {
        animIndices = animIdle;
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
