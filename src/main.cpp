
#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

// stlib
#include <chrono>
#include <common.hpp>

// internal
#include <world_system.hpp>
#include <render_system.hpp>
#include <physics_system.hpp>

using Clock = std::chrono::high_resolution_clock;
// Entry point
int main()
{
	// Global Systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
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
	world.init(&renderer, window, &camera, &physics);

	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();
		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		if (!world.game_over && !world.is_paused) {
            world.step(elapsed_ms);
            physics.step(elapsed_ms);
			renderer.step(elapsed_ms);
            world.handle_collisions();
        } 

		renderer.draw();
		
	}

	return 0;
}
