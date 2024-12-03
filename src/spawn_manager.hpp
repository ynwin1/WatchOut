#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <world_init.hpp>
#include <sound_system.hpp>
#include <particle_system.hpp>

class SpawnManager
{
public:
	void init(Camera* camera, SoundSystem* soundSystem, ParticleSystem* particleSystem);

	void step(float elapsed_ms);

	void resetSpawnSystem();

	void setTutorialMode(bool mode);

private:
	Camera* camera;
	SoundSystem* soundSystem;
	ParticleSystem* particleSystem;

	bool isTutorialModeOn = false;
	int currentEnemyIdx = 0;
	float initialSpawnTime = 0.f;
	const float initialSpawnInterval = 15000.f; // 15 seconds

	const float difficultyInterval = 120000.f;
	float difficultyTime = 10000.f;

	// Constants
	std::vector<std::string> entity_types = {
		"bird",
		"boar",
		"barbarian",
		"archer",
		"wizard",
		"troll",
		"bomber",
		"heart", // collectible
		"collectible_trap" // collectible
	};

	const std::unordered_map<std::string, float> spawn_delays_og = {
		{"boar", 30000.0},
		{"barbarian", 40000.0f},
		{"archer", 50000.0f},
		{"bird", 20000.0f},
		{"wizard", 60000.0f},
		{"troll", 70000.0f},
		{"bomber", 90000.0f},
		{"heart", 10000.0f},
		{"collectible_trap", 10000.0f}
	};

	std::unordered_map<std::string, float> spawn_delays = {
		{"boar", 30000.0},
		{"barbarian", 40000.0f},
		{"archer", 50000.0f},
		{"bird", 20000.0f},
		{"wizard", 60000.0f},
		{"troll", 70000.0f},
		{"bomber", 90000.0f},
		{"heart", 10000.0f},
		{"collectible_trap", 10000.0f}
	};

	std::unordered_map<std::string, float> next_spawn = {
		{"boar", 30000.0},
		{"barbarian", 40000.0f},
		{"archer", 50000.0f},
		{"bird", 20000.0f},
		{"wizard", 60000.0f},
		{"troll", 70000.0f},
		{"bomber", 90000.0f},
		{"heart", 10000.0f},
		{"collectible_trap", 10000.0f}
	};

	// By how many entities to increase at spawn delay
	const std::unordered_map<std::string, int> spawn_size = {
		{"boar", 2},
		{"barbarian", 2},
		{"archer", 1},
		{"bird", 2},
		{"wizard", 1},
		{"troll", 1},
		{"bomber", 1},
		{"heart", 2},
		{"collectible_trap", 2}
	};

	const std::unordered_map<std::string, int> max_entities_og = {
		{"boar", 5},
		{"barbarian", 4},
		{"archer", 3},
		{"bird", 8},
		{"wizard", 4},
		{"troll", 5},
		{"bomber", 3},
		{"heart", 2},
		{"collectible_trap", 2}
	};

	std::unordered_map<std::string, int> max_entities = {
		{"boar", 5},
		{"barbarian", 4},
		{"archer", 3},
		{"bird", 8},
		{"wizard", 4},
		{"troll", 5},
		{"bomber", 3},
		{"heart", 2},
		{"collectible_trap", 2}
	};

	using spawn_func = Entity(*)(vec2);
	const std::unordered_map<std::string, spawn_func> spawn_functions = {
		{"boar", createBoar},
		{"barbarian", createBarbarian},
		{"archer", createArcher},
		{"bird", createBird},
		{"wizard", createWizard},
		{"troll", createTroll},
		{"bomber", createBomber},
		{"heart", createHeart},
		{"collectible_trap", createCollectibleTrap}
	};

	vec2 get_spawn_location(const std::string& entity_type, bool initial);
	bool hasAllEnemiesSpawned();

	void initialSpawn(float elapsed_ms);
	void inGameSpawn(float elapsed_ms);

	void spawnEnemies(float elapsed_ms);
	void spawnCollectibles(float elapsed_ms);
	void spawnCollectible(std::string collectible, float elapsed_ms);
	void spawnParticles(float elapsed_ms);

	void despawnCollectibles(float elapsed_ms);
	void despawnTraps(float elapsed_ms);

	void adjustDifficulty(float elapsed_ms);

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;

};

