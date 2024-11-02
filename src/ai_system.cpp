#include "ai_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"


AISystem::AISystem(std::default_random_engine& rng)
{
    this->rng = rng;
}

void AISystem::moveTowardsPlayer(Entity enemy, vec3 playerPosition)
{
    Motion& enemyMotion = registry.motions.get(enemy);
    vec2 direction = normalize(playerPosition - enemyMotion.position);
    float speed = registry.enemies.get(enemy).speed;
    enemyMotion.velocity = vec3(direction * speed, enemyMotion.velocity.z);
}

#include <iostream>

void AISystem::boarBehaviour(Entity boar, vec3 playerPosition, float elapsed_ms)
{
    const float BOAR_AGGRO_RANGE = 500;
    const float BOAR_DISENGAGE_RANGE = 700;
    const float BOAR_PREPARE_TIME = 500; 
    const float BOAR_CHARGE_DURATION = 3000; 
    const float BOAR_COOLDOWN_TIME = 4000; 
    const float BOAR_CHARGE_SPEED = 2.0f; 

    if (registry.deathTimers.has(boar)) {
        return;
    }

    Motion& motion = registry.motions.get(boar);
    Boar& boars = registry.boars.get(boar);
    float distanceToPlayer = distance(motion.position, playerPosition);

    if (boars.cooldownTimer > 0) {
        boars.cooldownTimer -= elapsed_ms;
        return;
    }

    // Set state based on distance
    if (distanceToPlayer < BOAR_AGGRO_RANGE && boars.cooldownTimer <= 0 && !boars.preparing && !boars.charging) {
        boars.preparing = true;
        boars.prepareTimer = BOAR_PREPARE_TIME;
        boars.chargeTimer = BOAR_CHARGE_DURATION;
    } else if (distanceToPlayer > BOAR_DISENGAGE_RANGE) {
        boars.preparing = false;
        boars.charging = false;
    }

    if (boars.preparing) {
        // Preparation shake
        if (boars.prepareTimer > 0) {
            boars.prepareTimer -= elapsed_ms;

            float shakeMagnitude = 5.0f;
            float offsetX = (uniform_dist(rng) - 0.5f) * shakeMagnitude;
            float offsetY = (uniform_dist(rng) - 0.5f) * shakeMagnitude;

            motion.position.x += offsetX;
            motion.position.y += offsetY;
            motion.velocity = vec3(0, 0, 0);

        } else {
            boars.preparing = false; 
            boars.charging = true;
            boars.chargeDirection = normalize(vec2(playerPosition) - vec2(motion.position));
            motion.velocity = vec3(boars.chargeDirection * BOAR_CHARGE_SPEED, 0);
        }
    }

    if (boars.charging) {
        if (boars.chargeTimer > 0) {
            boars.chargeTimer -= elapsed_ms;
            motion.position += motion.velocity * (elapsed_ms / 1000.0f);

        } else {
            boars.charging = false;
            boars.cooldownTimer = BOAR_COOLDOWN_TIME;
            motion.velocity = vec3(0, 0, 0);
        }
    } else if (!boars.preparing) {
        moveTowardsPlayer(boar, playerPosition);
    }
}



void AISystem::barbarianBehaviour(Entity barbarian, vec3 playerPosition)
{
    if (registry.deathTimers.has(barbarian)) {
        return;
    }
    moveTowardsPlayer(barbarian, playerPosition);
}

void AISystem::shootArrow(Entity shooter, vec3 targetPos)
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

    // Prevent trying to shoot above what is possible
    if (vertical_distance >= horizontal_distance)
        return;

    float velocity = horizontal_distance * sqrt(-GRAVITATIONAL_CONSTANT / (vertical_distance - horizontal_distance));

    // Prevent shooting at crazy speeds
    if (velocity > MAX_ARROW_VELOCITY)
        return;

    // Introduce some randomness in the velocity 
    float random_factor = 1 + (0.5 - uniform_dist(rng)) / 20; // 0.975-1.025
    velocity *= random_factor;

    // Determine velocities for each dimension
    vec2 horizontal_velocity = velocity * cos(ARROW_ANGLE) * horizontal_direction;
    float vertical_velocity = velocity * sin(ARROW_ANGLE);
    createArrow(pos, vec3(horizontal_velocity, vertical_velocity));
}

void AISystem::archerBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms)
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
            boarBehaviour(enemy, playerPosition, elapsed_ms);
        }
        else if (registry.barbarians.has(enemy)) {
            barbarianBehaviour(enemy, playerPosition);
        }
        else if (registry.archers.has(enemy)) {
            archerBehaviour(enemy, playerPosition, elapsed_ms);
        }
    }
}
