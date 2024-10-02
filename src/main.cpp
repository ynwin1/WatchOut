
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <common.hpp>

// internal
#include <world_system.hpp>
#include <render_system.hpp>

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global Systems
	WorldSystem world;
	RenderSystem renderer;

	// Initializing window
	GLFWwindow* window = renderer.create_window();
	if (!window) {
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// Initialize the main systems
	renderer.init(window);
	world.init(&renderer, window);

	world.createJeff(&renderer, {0,0});

	auto t = Clock::now();
	int counter = 0;
	while (!world.is_over()) {
		std::cout << "In main loop: " << counter << std::endl;
		counter++;
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();
		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.step(elapsed_ms);
		//physics.step(elapsed_ms);
		world.handle_collisions();

		renderer.draw();
	}
	std::cout << "Exiting" << std::endl;
	return EXIT_SUCCESS;
}
