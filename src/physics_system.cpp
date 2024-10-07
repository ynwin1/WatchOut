#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>

bool collides(const Hitbox& a, const Hitbox& b)
{
	// position represents the center of the entity
	// dimension represents the width and height of entity

	// If a's bottom is higher than b's top
	if (a.position.y > b.position.y + (b.dimension.y / 2.0f))
		return false;
	// If a's top is lower than b's bottom
	if (a.position.y + (a.dimension.y / 2.0f) < b.position.y)
		return false;
	// If a's right is to the left of b's left
	if (a.position.x > b.position.x + (b.dimension.x / 2.0f))
		return false;
	// Check if a's left is to the right of b's right
	if (a.position.x + (a.dimension.x / 2.0f) < b.position.x)
		return false;

	return true;
}

void handleBoundsCheck() {
	ComponentContainer<Motion> &motion_container = registry.motions;

	for (uint i = 0; i < motion_container.components.size(); i++) {
		Motion& motion = motion_container.components[i];
		float halfScaleX = motion.scale.x / 2;
		float halfScaleY = motion.scale.y / 2;

		if(motion.position.x - halfScaleX < 0) {
			motion.position.x = halfScaleX;
		} else if(motion.position.x + halfScaleX > world_size_x) {
			motion.position.x = world_size_x - halfScaleX;
		}

		if(motion.position.y - halfScaleY < 0) {
			motion.position.y = halfScaleY;
		} else if(motion.position.y + halfScaleY > world_size_y) {
			motion.position.y = world_size_y - halfScaleY;
		}
	}
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Check for collisions between moving entities
	ComponentContainer<Motion> &motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++) {
		Entity entity_i = motion_container.entities[i];
		Hitbox& hitbox_i = registry.hitboxes.get(entity_i);

		for (uint j = i + 1; j < motion_container.components.size(); j++) {
			Entity entity_j = motion_container.entities[j];
			Hitbox& hitbox_j = registry.hitboxes.get(entity_j);

			if (collides(hitbox_i, hitbox_j)) {
				// Collision detected
				// std::cout << "Collision detected between entities " << entity_i << " and " << entity_j << std::endl;
				registry.collisions.emplace(entity_i, entity_j);
				registry.collisions.emplace(entity_j, entity_i);
			}
		}
	}

	handleBoundsCheck();
};