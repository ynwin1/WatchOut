#include "ai_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"

void moveTowardsPlayer(Entity enemy, vec3 playerPosition)
{
    Motion& enemyMotion = registry.motions.get(enemy);
    vec2 direction = normalize(playerPosition - enemyMotion.position);
    float speed = registry.enemies.get(enemy).speed;
    enemyMotion.velocity = vec3(direction * speed, enemyMotion.velocity.z);
}

void boarBehaviour(Entity boar, vec3 playerPosition)
{
    if (registry.deathTimers.has(boar)) {
        return;
    }
    moveTowardsPlayer(boar, playerPosition);
}

void barbarianBehaviour(Entity barbarian, vec3 playerPosition)
{
    if (registry.deathTimers.has(barbarian)) {
        return;
    }
    moveTowardsPlayer(barbarian, playerPosition);
}

void shootArrow(Entity shooter, vec3 targetPos)
{
    // Always shoot arrow at 45 degree angle (makes calculations simpler)
    const float ARROW_ANGLE = M_PI / 4;
    const float MAX_ARROW_VELOCITY = 10;

    Motion& motion = registry.motions.get(shooter);

    // Get start position of the arrow
    vec2 horizontal_direction = normalize(vec2(targetPos) - vec2(motion.position));
    const float maxArrowDimension = max(ARROW_BB_HEIGHT, ARROW_BB_WIDTH);
    vec3 pos = motion.position;
    if (abs(horizontal_direction.x) > abs(horizontal_direction.y)) {
        if (horizontal_direction.x > 0) {
            pos.x += motion.hitbox.x / 2 + maxArrowDimension;
        }
        else if (horizontal_direction.x < 0) {
            pos.x -= motion.hitbox.x / 2 + maxArrowDimension;
        }
    }
    else {
        if (horizontal_direction.y > 0) {
            pos.y += motion.hitbox.y / 2 + maxArrowDimension;
        }
        else if (horizontal_direction.x < 0) {
            pos.y -= motion.hitbox.y / 2 + maxArrowDimension;
        }
    }
    pos.z += motion.hitbox.z / 2 + maxArrowDimension;
    horizontal_direction = normalize(vec2(targetPos) - vec2(pos));

    // Get distances from start to target
    float horizontal_distance = distance(vec2(pos), vec2(targetPos));
    float vertical_distance = targetPos.z - pos.z;
    
    if (vertical_distance >= horizontal_distance) 
        return;

    float velocity = horizontal_distance * sqrt(-GRAVITATIONAL_CONSTANT / (vertical_distance - horizontal_distance));
    if (velocity > MAX_ARROW_VELOCITY)
        return;
    vec2 horizontal_velocity = velocity * cos(ARROW_ANGLE) * horizontal_direction;
    float vertical_velocity = velocity * sin(ARROW_ANGLE);
    createArrow(pos, vec3(horizontal_velocity, vertical_velocity));
}

void archerBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms)
{
    const float ARCHER_RANGE = 600;
    const float DISENGAGE_RANGE = 800;
    const float DRAW_ARROW_TIME = 2000;
    if (registry.deathTimers.has(entity)) {
        return;
    }
    Motion& motion = registry.motions.get(entity);
    Archer& archer = registry.archers.get(entity);
    float d = distance(motion.position, playerPosition);
    if (d < ARCHER_RANGE) {
        archer.aiming = true;
        motion.velocity.x = 0;
        motion.velocity.y = 0;
    }
    else if (d > DISENGAGE_RANGE) {
        archer.aiming = false;
        archer.drawArrowTime = 0;
    }

    if (archer.aiming) {
        if (archer.drawArrowTime > DRAW_ARROW_TIME) {
            shootArrow(entity, playerPosition);
            archer.drawArrowTime = 0;
        }
        else {
            archer.drawArrowTime += elapsed_ms;
        }
    }
    else {
        moveTowardsPlayer(entity, playerPosition);
    }
}


void AISystem::step(float elapsed_ms)
{
    // Skip if there is no player to pursue
    if (registry.players.entities.size() < 1) {
        return;
    }
    vec3 playerPosition = registry.motions.get(registry.players.entities.at(0)).position;

    for (Entity enemy : registry.enemies.entities) {
        if (registry.boars.has(enemy)) {
            boarBehaviour(enemy, playerPosition);
        }
        else if (registry.barbarians.has(enemy)) {
            barbarianBehaviour(enemy, playerPosition);
        }
        else if (registry.archers.has(enemy)) {
            archerBehaviour(enemy, playerPosition, elapsed_ms);
        }
    }
}
