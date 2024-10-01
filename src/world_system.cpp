#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"


WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::init(RenderSystem* renderer, GLFWwindow* window)
{
	this->renderer = renderer;
	this->window = window;
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
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
		
		// If the entity is a player
		if (registry.players.has(entity)) {
			// If the entity is colliding with a collectible
			if (registry.collectibles.has(entity_other)) {
				// TODO: [WO-17] Logic to collect the collectibles
			}
			else if (registry.traps.has(entity_other)) {
				// TODO: [WO-14] If the other entity is a damage trap

			}
			else if (registry.enemies.has(entity_other)) {
				// TODO: [WO-16] If the other entity is an enemy
			}
		}
		else if (registry.enemies.has(entity)) {
			// TODO: [Enemy - Others] Collision Logics go here
		}

	}
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
}

void WorldSystem::restart_game()
{
}
