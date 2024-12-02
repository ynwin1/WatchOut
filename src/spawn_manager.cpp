#include <iostream>
#include <iomanip> 
#include <sstream>

#include <vector>
#include <string>
#include <unordered_map>

#include "spawn_manager.hpp"
#include "common.hpp"

void SpawnManager::init(Camera* camera)
{
	this->camera = camera;
}

vec2 SpawnManager::get_spawn_location(const std::string& entity_type)
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
    return spawn_location;
}

bool SpawnManager::hasAllEnemiesSpawned() {
	return currentEnemyIdx >= entity_types.size() - 2; // there are 2 collectibles
}

void SpawnManager::initialSpawn(float elapsed_ms) {
    int maxEntitySize = 1;

    for (int i = 0; i <= currentEnemyIdx; i++) {
        std::string entity_type = entity_types[i];
        int currentEntitySize = registry.spawnable_lists.at(entity_type)->size();
        if (currentEntitySize < maxEntitySize) {
            vec2 spawn_location = get_spawn_location(entity_type);
            spawn_func f = spawn_functions.at(entity_type);
            (*f)(spawn_location);
        }
    }

	spawnCollectibles(elapsed_ms);

    // spawn new enemy
    if (initialSpawnTime <= 0) {
        // spawn current enemy
		std::string entity_type = entity_types[currentEnemyIdx];
		vec2 spawn_location = get_spawn_location(entity_type);
		spawn_func f = spawn_functions.at(entity_type);
		(*f)(spawn_location);
		currentEnemyIdx++;
		initialSpawnTime = initialSpawnInterval;
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
                vec2 spawn_location = get_spawn_location(entity_type);
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
        vec2 trap_spawn_location = get_spawn_location(collectible);
        spawn_func f = spawn_functions.at(collectible);
        (*f)(trap_spawn_location);
        next_spawn.at(collectible) = spawn_delays.at(collectible);
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
};