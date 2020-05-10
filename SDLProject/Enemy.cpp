#include "Enemy.h"

Enemy::Enemy() noexcept
{
    position = glm::vec3(0); //coords
    movement = glm::vec3(0); //direction of moving
    acceleration = glm::vec3(0); //how fast speed increases
    velocity = glm::vec3(0); //how fast the entity is going
    speed = 0; // a scalar of how fast the entity is going
    
    modelMatrix = glm::mat4(1.0f);
    
    type = ENEMY;
    aiType = JUMPER;
    aiState = IDLE;
}

bool Enemy::CheckCollision(Entity *other) {
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
 
void Enemy::CheckCollisionsY(Entity *objects, int objectCount) {
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

void Enemy::CheckCollisionsX(Entity *objects, int objectCount) {
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

void Enemy::CheckCollisionsY(Map *map) {
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

void Enemy::CheckCollisionsX(Map *map) {
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

void Enemy::AIWalker () {
    movement = glm::vec3(-1.0f,0.0f,0.0f);
}

void Enemy::AIJumper (Entity *player) {
    switch (aiState)
    {
        case IDLE: //starts in IDLE
            //jump = true;
            if (glm::distance(position, player->position) <5.0f) {
                aiState = WALKING;
            }
            break;
        case WALKING:
            if (player->position.x < position.x) {
                movement = glm::vec3(-1.0f,0.0f,0.0f);
            } else {
                movement = glm::vec3(1.0f,0.0f,0.0f);
            }
            aiState=JUMPING;
            break;
        case JUMPING:
            /*if (jumpCount/40 > 1) {
                jump = true;
                jumpCount = 0;
            }*/
            if (player->position.x < position.x) {
                movement = glm::vec3(-1.0f,0.0f,0.0f);
            } else {
                movement = glm::vec3(1.0f,0.0f,0.0f);
            }
            aiState = WALKING;
            break;
        case DEAD:
            isActive = false;
            return;
            break;
    }
}

void Enemy::AIStalker (Entity *player) {
    //elapsed_time = difftime(start,elapsed_time);
    switch (aiState)
    {
        case IDLE: //starts in IDLE
            if (glm::distance(position, player->position) >1.0f)
                aiState=WALKING;
            break;
        case WALKING:
            if (glm::distance(position, player->position) >1.0f)
            {
                if (player->position.x < position.x) {
                    movement = glm::vec3(-1.0f,0.0f,0.0f);
                } else {
                    movement = glm::vec3(1.0f,0.0f,0.0f);
                }
            } else
                aiState=WALKING;
            break;
        case STALKING:
            if (glm::distance(position, player->position) <1.0f && glm::distance(position, player->position) > 0.001f)
            {
                if (player->position.x < position.x) {
                    movement = glm::vec3(1.0f,0.0f,0.0f);
                } else {
                    movement = glm::vec3(-1.0f,0.0f,0.0f);
                }
            }
            else
                aiState = WALKING;
            break;
        case DEAD:
            isActive = false;
            return;
            break;
    }
}


void Enemy::AIFollower(Entity *player, Entity *enemies, int enemyCount){//unused
    switch (aiState)
    {
        case LEADING:
            break;
        case FOLLOWING:
            if (player->position.x < position.x) {
                movement = glm::vec3(-1.0f,0.0f,0.0f);
            } else {
                movement = glm::vec3(1.0f,0.0f,0.0f);
            }
            if (player->position.y < position.y)
                aiState = JUMPING;
            break;
        case JUMPING:
            /*if (jumpCount/40 > 1) {
                jump = true;
                jumpCount = 0;
            }*/
            if (player->position.x < position.x) {
                movement = glm::vec3(-3.0f,0.0f,0.0f);
            } else {
                movement = glm::vec3(3.0f,0.0f,0.0f);
            }
            aiState = WALKING;
            break;
        case DEAD:
            isActive = false;
            return;
            break;
    }
}
               
void Enemy::AI(Entity *player, Entity *enemies, int enemyCount) {
    if (!isActive) return;
    switch(aiType) {
        case WALKER:
            AIWalker();
            break;
        case JUMPER:
            //jumpPower = 2.0f;
            AIJumper(player);
            break;
        case STALKER:
            AIStalker(player);
            break;
        case FOLLOWER://unused
            AIFollower(player, enemies, enemyCount);
    }
}

void Enemy::Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Map *map)
{
    
    if (type == ENEMY && isActive) {
        //jumpCount++;
        CheckCollision(player);
        if (lastCollided == PLAYER && (collidedTop )){//|| collidedTopRight || collidedTopLeft)) {
            isActive = false;
            aiState = DEAD;
            //player->jump = true;
        } else if (!player->invulnerability && player->invulnerabilityCount == 0 && lastCollided == PLAYER && (collidedLeft || collidedRight || collidedBottom || collidedBottomLeft || collidedBottomRight)){
            player->health -= 1;
            lastCollided = AIR;
            player->invulnerability = true;
        }
        AI(player, enemies, enemyCount);
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
            animIndex = 0;
        }
    }
    
    /*if (jump) { //instant VELOCTIY!
        jump = false;
        velocity.y += jumpPower;
        if(type == PLAYER) {
            Mix_VolumeMusic(MIX_MAX_VOLUME/4);
            Mix_Chunk *bounce = Mix_LoadWAV("Bounce.wav");
            Mix_PlayChannel(-1, bounce, 0);
        }
    }*/
    
    velocity.x = movement.x * speed; //if the player is tryna move, let them move
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

void Enemy::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
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

void Enemy::Render(ShaderProgram *program) {
    if (isActive == false && type != PLATFORM) return;
    if (type == PLAYER && invulnerability && invulnerabilityCount%2)
        program->SetModelMatrix(modelMatrix);
    else if (type == PLAYER && invulnerability)
        return;
    else
        program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
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
