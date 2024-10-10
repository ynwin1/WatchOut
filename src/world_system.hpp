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
	WorldSystem();

	// starts the game
	void init(RenderSystem* renderer, GLFWwindow* window, Camera* camera, PhysicsSystem* physics);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	bool game_over;

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

private:
	// GLFW Window handle
	GLFWwindow* window;

	RenderSystem* renderer;
	PhysicsSystem* physics;

	Camera* camera;

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_position);
	
	Entity playerEntity;
	std::vector<std::string> entity_types;
	const std::unordered_map<std::string, float> spawn_delays;
	const std::unordered_map<std::string, unsigned int> max_entities;
	std::unordered_map<std::string, float> next_spawns;

	using spawn_func = Entity (*)(RenderSystem*, vec2);
	const std::unordered_map<std::string, spawn_func> spawn_functions;

	// restart level
	void restart_game();

	// Actions performed for each step
	void update_positions(float elapsed_ms);
	void update_player_facing(Player& player);
	void update_cooldown(float elapsed_ms);
	void handle_deaths(float elapsed_ms);
	void spawn(float elapsed_ms);
	void think();
	void recoil_entities(Motion& motion1, Motion& motion2);
	float calculate_x_overlap(Motion& motion1, Motion& motion2);
	float calculate_y_overlap(Motion& motion1, Motion& motion2);


	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
