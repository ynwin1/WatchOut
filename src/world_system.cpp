#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"


WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::init()
{
}

WorldSystem::~WorldSystem() {
	// Destroy all created components
	registry.clear_all_components();
}

bool WorldSystem::step(float elapsed_ms)
{
	return false;
}

void WorldSystem::handle_collisions()
{
}

bool WorldSystem::is_over() const
{
	return false;
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
}

void WorldSystem::restart_game()
{
}
