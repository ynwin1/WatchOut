#pragma once

// stlib
#include <vector>
#include <random>

// internal 
#include <render_system.hpp>


// Container for all our entities and game logic
class WorldSystem
{
public:
	WorldSystem();

	// starts the game
	void init(RenderSystem* renderer, GLFWwindow* window);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

	Entity createJeff(RenderSystem* renderer, vec2 position);

private:
	// GLFW Window handle
	GLFWwindow* window;

	RenderSystem* renderer;

	// restart level
	void restart_game();

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
