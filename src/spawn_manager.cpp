#include <iostream>
#include <iomanip> 
#include <sstream>

#include <vector>
#include <string>
#include <unordered_map>

#include "spawn_manager.hpp"
#include "particle_system.hpp"
#include "common.hpp"

void SpawnManager::init(Camera* camera, SoundSystem* soundSystem, ParticleSystem* particleSystem)
{
	this->camera = camera;
    this->soundSystem = soundSystem;
	this->particleSystem = particleSystem;
}

vec2 SpawnManager::get_spawn_location(const std::string& entity_type, bool initial)
{
    vec2 spawn_location{};
    // spawn collectibles
    if (entity_type == "heart" || entity_type == "collectible_trap") {
        // spawn at random location on the map
        float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
        float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
        spawn_location = { posX, posY };
    }
    else
        // spawn enemies
    {
        if (initial) {
			// spawn within camera's view
			float exclusionTop = (camera->getPosition().y - camera->getSize().y / 2) / yConversionFactor - 100;
			float exclusionBottom = (camera->getPosition().y + camera->getSize().y / 2) / yConversionFactor + 400;
			float exclusionLeft = camera->getPosition().x - camera->getSize().x / 2 + 100;
			float exclusionRight = camera->getPosition().x + camera->getSize().x / 2 - 100;

            // only within camera bound
			float posX = uniform_dist(rng) * (exclusionRight - exclusionLeft) + exclusionLeft;
			float posY = uniform_dist(rng) * (exclusionBottom - exclusionTop) + exclusionTop;

            spawn_location = { posX, posY };
        }
        else {
            // Do not spawn within camera's view (with some margins)
            float exclusionTop = (camera->getPosition().y - camera->getSize().y / 2) / yConversionFactor - 100;
            float exclusionBottom = (camera->getPosition().y + camera->getSize().y / 2) / yConversionFactor + 400;
            float exclusionLeft = camera->getPosition().x - camera->getSize().x / 2 - 100;
            float exclusionRight = camera->getPosition().x + camera->getSize().x / 2 + 100;

            float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
            float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
            // Spawning in exclusion zone
            if (posX < exclusionRight && posX > exclusionLeft &&
                posY < exclusionBottom && posY > exclusionTop)
            {
                if (exclusionTop > topBound) {
                    posY = exclusionTop;
                }
                else {
                    posY = exclusionBottom;
                }
            }
            spawn_location = { posX, posY };
        }
    }
    return spawn_location;
}

bool SpawnManager::hasAllEnemiesSpawned() {
	return currentEnemyIdx >= entity_types.size() - 2; // there are 2 collectibles
}

void SpawnManager::initialSpawn(float elapsed_ms) {
    int maxEntitySize = 1;

    for (int i = 0; i < currentEnemyIdx; i++) {
        std::string entity_type = entity_types[i];
        int currentEntitySize = registry.spawnable_lists.at(entity_type)->size();
        if (currentEntitySize < maxEntitySize) {
            vec2 spawn_location = get_spawn_location(entity_type, true);
            spawn_func f = spawn_functions.at(entity_type);
            (*f)(spawn_location);
            std::cout << "Spawning " + entity_type << std::endl;
        }
    }

	spawnCollectibles(elapsed_ms);

    // spawn new enemy
    if (initialSpawnTime <= 0) {
        // spawn current enemy
		std::string entity_type = entity_types[currentEnemyIdx];
		vec2 spawn_location = get_spawn_location(entity_type, true);
		spawn_func f = spawn_functions.at(entity_type);
		(*f)(spawn_location);
		currentEnemyIdx++;
		initialSpawnTime = initialSpawnInterval;
        soundSystem->playSoundEffect(Sound::LEVELUP, 0);
    }
    else {
		initialSpawnTime -= elapsed_ms;
    }
}

void SpawnManager::inGameSpawn(float elapsed_ms) {
	spawnEnemies(elapsed_ms);
	spawnCollectibles(elapsed_ms);
}

void SpawnManager::spawnEnemies(float elapsed_ms) {
    for (int i = 0; i < entity_types.size(); i++) {
        std::string entity_type = entity_types[i];

		next_spawn.at(entity_type) -= elapsed_ms;

		bool isWithinMaxSize = registry.spawnable_lists.at(entity_type)->size() < max_entities.at(entity_type);
		bool hasReachedSpawnTime = next_spawn.at(entity_type) <= 0;
        if (isWithinMaxSize && hasReachedSpawnTime) {
            
			int num_to_spawn = spawn_size.at(entity_type);
            spawn_func f = spawn_functions.at(entity_type);
            for (int j = 0; j < num_to_spawn; j++) {
                vec2 spawn_location = get_spawn_location(entity_type, false);
                (*f)(spawn_location);
            }

			next_spawn.at(entity_type) = spawn_delays.at(entity_type);
        }
    }
}

void SpawnManager::spawnCollectibles(float elapsed_ms) {
	// collectible
	spawnCollectible("collectible_trap", elapsed_ms); // collectible_trap
    // heart
	spawnCollectible("heart", elapsed_ms); // heart
}

void SpawnManager::spawnCollectible(std::string collectible, float elapsed_ms) {
    next_spawn.at(collectible) -= elapsed_ms;
    if (next_spawn.at(collectible) <= 0) {
        vec2 trap_spawn_location = get_spawn_location(collectible, false);
        spawn_func f = spawn_functions.at(collectible);
        (*f)(trap_spawn_location);
        next_spawn.at(collectible) = spawn_delays.at(collectible);
    }
}

void SpawnManager::spawnParticles(float elapsed_ms)
{
    Entity& player = registry.players.entities[0];
    Motion& playerMotion = registry.motions.get(player);

    // SPAWN SMOKE ------------------------------------------------
    vec3 position = playerMotion.position;
    float direction = (playerMotion.scale.x > 0) ? 1.f : -1.f;
    position.x += direction * playerMotion.hitbox.x / 2;
    position.z += playerMotion.hitbox.z / 3;
    vec2 size = { 20, 20 };
	particleSystem->createSmokeParticle(position, size);
    particleSystem->createSmokeParticle(position, size);
    particleSystem->createSmokeParticle(position, size);

    for (Entity fireball : registry.damagings.entities) {
        Damaging& damaging = registry.damagings.get(fireball);
        if (damaging.type != "fireball") {
            continue;
        }
        vec3 position = registry.motions.get(fireball).position;
        vec2 size = { 50, 50 };
        particleSystem->createSmokeParticle(position, size);
        particleSystem->createSmokeParticle(position, size);
        particleSystem->createSmokeParticle(position, size);
        particleSystem->createSmokeParticle(position, size);
    }

    // SPAWN DASH SPRITES ------------------------------------------------
    if (registry.dashers.get(player).isDashing) {
        float facing = playerMotion.scale.x > 0 ? 1 : -1;
        particleSystem->createDashParticle(playerMotion.position, vec2(JEFF_BB_WIDTH * facing, JEFF_BB_HEIGHT));
    }
}

void SpawnManager::despawnCollectibles(float elapsed_ms) {
    for (auto& collectibleEntity : registry.collectibles.entities) {
        Collectible& collectible = registry.collectibles.get(collectibleEntity);
        collectible.timer += elapsed_ms;

        if (collectible.timer >= collectible.duration) {
            registry.remove_all_components_of(collectibleEntity);
        }
        else if (collectible.timer >= collectible.duration / 2) {
            AnimationController& animatedCollectible = registry.animationControllers.get(collectibleEntity);
            if (animatedCollectible.currentState != AnimationState::Fading) {
                animatedCollectible.changeState(collectibleEntity, AnimationState::Fading);
            }
        }
    }
}

void SpawnManager::despawnTraps(float elapsed_ms) {
    for (Entity& trapE : registry.traps.entities) {
        Trap& trap = registry.traps.get(trapE);
        trap.duration -= elapsed_ms;
        if (trap.duration <= 0) {
            registry.remove_all_components_of(trapE);
        }
    }

    for (Entity& trapE : registry.phantomTraps.entities) {
        PhantomTrap& trap = registry.phantomTraps.get(trapE);
        trap.duration -= elapsed_ms;
        if (trap.duration <= 0) {
            registry.remove_all_components_of(trapE);
        }
    }
}

void SpawnManager::resetSpawnSystem() {
    currentEnemyIdx = 0;
    initialSpawnTime = 0.f;
    for (auto& entity_spawn : next_spawn) {
        entity_spawn.second = spawn_delays.at(entity_spawn.first);
    }
}

void SpawnManager::step(float elapsed_ms) {
    if (!hasAllEnemiesSpawned()) {
        initialSpawn(elapsed_ms);
    }
    else {
        inGameSpawn(elapsed_ms);
    }
	spawnParticles(elapsed_ms);
	despawnCollectibles(elapsed_ms);
    despawnTraps(elapsed_ms);
};