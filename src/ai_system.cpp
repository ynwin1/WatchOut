#include "ai_system.hpp"

void boarBehaviour(Entity boar, vec3 playerPosition)
{
    if (registry.deathTimers.has(boar)) {
        return;
    }
    Motion& enemyMotion = registry.motions.get(boar);
    vec2 direction = normalize(playerPosition - enemyMotion.position);
    float speed = registry.enemies.get(boar).speed;
    enemyMotion.velocity = vec3(direction * speed, enemyMotion.velocity.z);
}

void barbarianBehaviour(Entity barbarian, vec3 playerPosition)
{
    if (registry.deathTimers.has(barbarian)) {
        return;
    }
    Motion& enemyMotion = registry.motions.get(barbarian);
    vec2 direction = normalize(playerPosition - enemyMotion.position);
    float speed = registry.enemies.get(barbarian).speed;
    enemyMotion.velocity = vec3(direction * speed, enemyMotion.velocity.z);
}

void archerBehaviour(Entity archer, vec3 playerPosition)
{
    if (registry.deathTimers.has(archer)) {
        return;
    }
    Motion& enemyMotion = registry.motions.get(archer);
    vec2 direction = normalize(playerPosition - enemyMotion.position);
    float speed = registry.enemies.get(archer).speed;
    enemyMotion.velocity = vec3(direction * speed, enemyMotion.velocity.z);
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
