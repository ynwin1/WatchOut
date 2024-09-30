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

	// Check for collisions
	void handle_collisions();

	// Handle player movement
    void update_player_movement(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

private:
	// GLFW Window handle
	GLFWwindow* window;

	RenderSystem* renderer;
	Entity playerEntity;

    // Input callback functions
    void on_key(int key, int, int action, int mod);

	// restart level
	void restart_game();

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
