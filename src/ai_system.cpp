#include "ai_system.hpp"
#include "world_init.hpp"

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

void archerBehaviour(Entity archer, vec3 playerPosition)
{
    const float ARCHER_RANGE = 500;
    if (registry.deathTimers.has(archer)) {
        return;
    }
    Motion& motion = registry.motions.get(archer);
    if (distance(motion.position, playerPosition) < ARCHER_RANGE) {
        // Shoot arrow

    }
    else {
        moveTowardsPlayer(archer, playerPosition);
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
            archerBehaviour(enemy, playerPosition);
        }
    }
}
