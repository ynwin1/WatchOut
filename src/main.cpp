
#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

// stlib
#include <chrono>
#include <common.hpp>
#include <random>

// internal
#include <world_system.hpp>
#include <render_system.hpp>
#include <physics_system.hpp>
#include <ai_system.hpp>
#include <sound_system.hpp>

using Clock = std::chrono::high_resolution_clock;
// Entry point
int main()
{
	std::default_random_engine rng = std::default_random_engine(std::random_device()());

	// Global Systems
	WorldSystem world = WorldSystem(rng);
	RenderSystem renderer;
	PhysicsSystem physics;
	SoundSystem sound;
	AISystem ai = AISystem(rng, &sound);
	Camera camera;
	

	// Initializing window
	GLFWwindow* window = renderer.create_window();
	if (!window) {
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// Initialize the main systems
	camera.init(window);
	renderer.init(&camera);
	world.init(&renderer, window, &camera, &physics, &ai, &sound);
	sound.init();

	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		if (world.gameStateController.getGameState() == GAME_STATE::PLAYING) {
			glfwPollEvents();
		}
		else {
			// Wait until an event when in a static mode
			glfwWaitEvents();
			t = Clock::now();
		}

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		GAME_STATE currentState = world.gameStateController.getGameState();
		if (currentState == GAME_STATE::PLAYING) {
			physics.step(elapsed_ms);
			world.step(elapsed_ms);
            world.handle_collisions();
			ai.step(elapsed_ms);
			renderer.step(elapsed_ms);
			sound.step(elapsed_ms);
		}

		/*if (currentState != GAME_STATE::HELP && currentState != GAME_STATE::PAUSED) {
			sound.step(elapsed_ms);
		}*/

		renderer.draw();
		
	}

	return 0;
}
