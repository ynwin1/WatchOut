#pragma once

// stlib
#include <vector>
#include <random>

// internal 
#include <render_system.hpp>
#include <physics_system.hpp>
#include <ai_system.hpp>
#include "game_state_controller.hpp"

// Container for all our entities and game logic
class WorldSystem
{
public:
	WorldSystem(std::default_random_engine& rng);

	// starts the game
	void init(RenderSystem* renderer, GLFWwindow* window, Camera* camera, PhysicsSystem* physics, AISystem* ai);

	// Releases all associated resources
	~WorldSystem();

	GameStateController gameStateController;

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	
	bool show_mesh;

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

	friend class GameStateController;

private:
	// CONSTANTS
	// Spawn delays
	const float ORIGINAL_BOAR_SPAWN_DELAY = 3000.0f;
	const float ORIGINAL_BABARIAN_SPAWN_DELAY = 5000.0f;
	const float ORIGINAL_ARCHER_SPAWN_DELAY = 7000.0f;
	const float ORIGINAL_HEART_SPAWN_DELAY = 10000.0f;
	const float ORIGINAL_TRAP_SPAWN_DELAY = 7000.0f;

	// Max entities
	const unsigned int MAX_BOARS = 1;
	const unsigned int MAX_BABARIANS = 1;
	const unsigned int MAX_ARCHERS = 0;
	const unsigned int MAX_HEARTS = 1;
	const unsigned int MAX_TRAPS = 1;

	const float DIFFICULTY_INTERVAL = 45000.0f;
	const unsigned int MAX_TOTAL_ENEMIES = 100;

	// GLFW Window handle
	GLFWwindow* window;
	RenderSystem* renderer;
	PhysicsSystem* physics;
	AISystem* ai;
	Camera* camera;
	TrapsCounter trapsCounter;

	bool isWindowed = false;

	Entity playerEntity;
	std::vector<std::string> entity_types;
	std::unordered_map<std::string, float> spawn_delays;
	std::unordered_map<std::string, unsigned int> max_entities;
	std::unordered_map<std::string, float> next_spawns;

	using spawn_func = Entity(*)(vec2);
	const std::unordered_map<std::string, spawn_func> spawn_functions;

	// Keeps track of what collisions have been handled recently.
	// Key uses entities cast to ints for comparisons.
	std::map<std::pair<int, int>, float> collisionCooldowns;

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_position);

	// restart level
	void restart_game();
	void initText();

	// Actions performed for each step
	void spawn(float elapsed_ms);
	void update_cooldown(float elapsed_ms);
	void handle_deaths(float elapsed_ms);
	void update_player_facing(Player& player, Motion& motion);
	void despawn_collectibles(float elapsed_ms);
	void handle_stamina(float elapsed_ms);
	vec2 get_spawn_location(const std::string& entity_type);
	void place_trap(Player& player, Motion& motion, bool forward);
	void checkAndHandlePlayerDeath(Entity& entity);
	void trackFPS(float elapsed_ms);
	void updateGameTimer(float elapsed_ms);
	void updateTrapsCounterText();
	void toggleMesh();
	void adjustSpawnSystem(float elapsed_ms);
	void resetSpawnSystem();
	void despawnTraps(float elapsed_ms);
	void updateCollectedTimer(float elapsed_ms);
	void resetTrappedEntities();

	// Collision functions
	void entity_collectible_collision(Entity entity, Entity collectible);
	void entity_trap_collision(Entity entity, Entity trap, std::vector<Entity>& was_damaged);
	void entity_damaging_collision(Entity entity, Entity trap, std::vector<Entity>& was_damaged);
	void entity_obstacle_collision(Entity entity, Entity obstacle, std::vector<Entity>& was_damaged);
	void moving_entities_collision(Entity entity, Entity entityOther, std::vector<Entity>& was_damaged);
	void processPlayerEnemyCollision(Entity player, Entity enemy, std::vector<Entity>& was_damaged);
	void processEnemyEnemyCollision(Entity enemy1, Entity enemy2, std::vector<Entity>& was_damaged);
	void handleEnemyCollision(Entity attacker, Entity target, std::vector<Entity>& was_damaged);
	void checkAndHandleEnemyDeath(Entity entity);
	void knock(Entity knocked, Entity knocker);

	// Help/Pause Menu functions
	Entity createHelpMenu(vec2 cameraPosition);
    void exitHelpMenu();
    Entity createPauseMenu(vec2 cameraPosition);
    void exitPauseMenu();

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
    // number between 0..1
};
