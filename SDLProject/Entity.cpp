#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0); //coords
    movement = glm::vec3(0); //direction of moving
    acceleration = glm::vec3(0); //how fast speed increases
    velocity = glm::vec3(0); //how fast the entity is going
    speed = 0; // a scalar of how fast the entity is going
    flag = GOING;
    
    modelMatrix = glm::mat4(1.0f);
}

void Entity::flipActive() {
    if (this)
    {
        if (isActive)
            isActive = false;
        else
            isActive = true;
    }
}

bool Entity::CheckCollision(Entity *other) {
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs (position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs (position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0){
        lastCollided = other->type;
        if (type == ENEMY) {
            if (velocity.y > 0 || other->velocity.y < 0) {
                collidedTop = other->type;
            } else if (velocity.y < 0) {
                collidedBottom = other->type;
            }
            if (velocity.x > 0) {
                collidedRight = other->type;
            } else if (velocity.x < 0) {
                collidedLeft = other->type;
            }
        }
        return true;
    }
    return false;
}
 
void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = object->type;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = object->type;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = object->type;
            } else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = object->type;
            }
        }
    }
}

void Entity::AIWalker () {
    movement = glm::vec3(-1.0f,0.0f,0.0f);
}

void Entity::AIWaitAndGo (Entity *player) {
    //jump = false;
    switch (aiState)
    {
        case IDLE:
            if (glm::distance(position, player->position) <3.0f) {
                aiState = WALKING;
            }
            break;
        case WALKING:
            if (player->position.x < position.x){
                movement = glm::vec3(-1.0f,0.0f,0.0f);
            } else {
                movement = glm::vec3(1.0f,0.0f,0.0f);
            }
            break;
        case JUMPING:
            break;
        case STALKING:
            break;
        case DEAD:
            isActive = false;
            return;
            break;
    }
}

void Entity::AIJumper (Entity *player) {
    switch (aiState)
    {
        case IDLE: //starts in IDLE
            jump = true;
            aiState=WALKING;
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
            if (player->position.x < position.x) {
                movement = glm::vec3(-1.0f,0.0f,0.0f);
            } else {
                movement = glm::vec3(1.0f,0.0f,0.0f);
            }
            aiState = WALKING;
            break;
        case STALKING:
            break;
        case DEAD:
            isActive = false;
            return;
            break;
    }
}

void Entity::AIStalker (Entity *player) {
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
        case JUMPING:
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
               
void Entity::AI(Entity *player, float deltaTime) {
    if (collidedTop == PLAYER) {
        isActive = false;
        aiState = DEAD;
        return;
    }
    switch(aiType) {
        case WALKER:
            AIWalker();
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case JUMPER:
            jumpPower = 2.0f;
            AIJumper(player);
            break;
        case STALKER:
            AIStalker(player);
            break;
    }
}

GameMode Entity::Update(float deltaTime, Entity *player, Entity *enemies, int enemyCount, Entity *platforms, int platformCount)
{
    if (type == ENEMY) {
        CheckCollision(player);
        if (collidedTop == PLAYER) {
            isActive = false;
            aiState = DEAD;
            player->jump = true;
        } else if (collidedLeft == PLAYER || collidedRight == PLAYER || collidedBottom == PLAYER ){
            player->flipActive();
            flag = FAILURE;
            return flag;
        }
    }
    int allDead = 0;
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].isActive == false)
            allDead++;
    }
    if (allDead == enemyCount) {
        player->flipActive();
        flag = SUCCESS;
        return flag;
    }
    collidedTop = NONE;
    collidedBottom = NONE;
    collidedLeft = NONE;
    collidedRight = NONE;
    
    if (type == ENEMY) {
        AI(player, deltaTime);
    }
    
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
    
    if (jump) { //instant VELOCTIY!
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed; //if the player is tryna move, let them move
    velocity += acceleration * deltaTime; //changes velocity in relation to time
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    
    //CheckCollisionsY(enemies,enemyCount);
    //CheckCollisionsX(enemies,enemyCount); //deletes enemies??
    
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    return flag;
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
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

void Entity::Render(ShaderProgram *program) {
    if (isActive == false && type != PLATFORM) return;
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
