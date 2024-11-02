#pragma once

// stlib
#include <vector>
#include <random>

// internal 
#include <render_system.hpp>
#include <physics_system.hpp>


// Container for all our entities and game logic
class WorldSystem
{
public:
	WorldSystem(std::default_random_engine& rng);

	// starts the game
	void init(RenderSystem* renderer, GLFWwindow* window, Camera* camera, PhysicsSystem* physics);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	bool game_over;
	bool is_paused;
	bool show_mesh;

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

private:
	// GLFW Window handle
	GLFWwindow* window;

	bool isWindowed = false;

	RenderSystem* renderer;
	PhysicsSystem* physics;

	Camera* camera;
	
	TrapsCounter trapsCounter;

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_position);
	
	Entity playerEntity;
	std::vector<std::string> entity_types;
	const std::unordered_map<std::string, float> spawn_delays;
	const std::unordered_map<std::string, unsigned int> max_entities;
	std::unordered_map<std::string, float> next_spawns;

	using spawn_func = Entity (*)(vec2);
	const std::unordered_map<std::string, spawn_func> spawn_functions;

	// restart level
	void restart_game();
	void initText();

	// Actions performed for each step
	void spawn(float elapsed_ms);
	void update_cooldown(float elapsed_ms);
	void handle_deaths(float elapsed_ms);
	void update_player_facing(Player& player);
	void despawn_collectibles(float elapsed_ms);
	void handle_stamina(float elapsed_ms);
	vec2 get_spawn_location(const std::string& entity_type);
	void place_trap(Player& player, Motion& motion, bool forward);
	void checkAndHandlePlayerDeath(Entity& entity);
	void trackFPS(float elapsed_ms);
	void updateGameTimer(float elapsed_ms);
	void updateTrapsCounterText();
	void toggleMesh();

	// Collision functions
	void entity_collectible_collision(Entity entity, Entity collectible);
	void entity_trap_collision(Entity entity, Entity trap, std::vector<Entity>& was_damaged);
	void entity_damaging_collision(Entity entity, Entity trap, std::vector<Entity>& was_damaged);
	void moving_entities_collision(Entity entity, Entity entityOther, std::vector<Entity>& was_damaged);
	void processPlayerEnemyCollision(Entity player, Entity enemy, std::vector<Entity>& was_damaged);
	void processEnemyEnemyCollision(Entity enemy1, Entity enemy2, std::vector<Entity>& was_damaged);
	void handleEnemyCollision(Entity attacker, Entity target, std::vector<Entity>& was_damaged);
	void checkAndHandleEnemyDeath(Entity entity);


	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
