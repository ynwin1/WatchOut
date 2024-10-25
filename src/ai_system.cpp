#include "ai_system.hpp"

void AISystem::step(float elapsed_ms)
{
    Motion& playerMotion = registry.motions.get(registry.players.entities.at(0));

    const float BOAR_SPEED = 0.4;
    for (Entity boar : registry.boars.entities) {
        if (registry.deathTimers.has(boar)) continue; // Skip dead boars
        Motion& enemyMotion = registry.motions.get(boar);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = vec3(direction * BOAR_SPEED, enemyMotion.velocity.z);
    }

    const float BARBARIAN_SPEED = 0.3;
    for (Entity enemy : registry.barbarians.entities) {
        if (registry.deathTimers.has(enemy)) continue; // Skip dead barbarians
        Motion& enemyMotion = registry.motions.get(enemy);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = vec3(direction * BARBARIAN_SPEED, enemyMotion.velocity.z);
    }

    const float ARCHER_SPEED = 0.2;
    for (Entity archer : registry.archers.entities) {
        if (registry.deathTimers.has(archer)) continue; // Skip dead archers
        Motion& enemyMotion = registry.motions.get(archer);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = vec3(direction * ARCHER_SPEED, enemyMotion.velocity.z);
    }
}
