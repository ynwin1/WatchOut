
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include <world_system.hpp>

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	WorldSystem world;

	auto t = Clock::now();
	while (!world.is_over()) {
		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.step(elapsed_ms);
		//physics.step(elapsed_ms);
		world.handle_collisions();

		//renderer.draw();
	}

	return 0;
}
