#include "physics_system.hpp"
#include "world_init.hpp"
#include "render_system.hpp"
#include <iostream>

bool collides(const Entity& a, const Entity& b)
{
	// position represents the center of the entity
	vec3 a_position = registry.motions.get(a).position;
	vec3 b_position = registry.motions.get(b).position;
	// dimension represents the width and height of entity
	vec3 a_dimension = registry.hitboxes.get(a).dimension;
	vec3 b_dimension = registry.hitboxes.get(b).dimension;

	// If a's bottom is higher than b's top
	if (a_position.y > b_position.y + ((b_dimension.y + a_dimension.y) / 2.0f))
		return false;
	// If a's top is lower than b's bottom
	if (a_position.y + ((a_dimension.y + b_dimension.y) / 2.0f) < b_position.y)
		return false;
	// If a's right is to the left of b's left
	if (a_position.x > b_position.x + ((b_dimension.x + a_dimension.x) / 2.0f))
		return false;
	// Check if a's left is to the right of b's right
	if (a_position.x + ((a_dimension.x + b_dimension.x) / 2.0f) < b_position.x)
		return false;

	return true;
}

void handleBoundsCheck() {
	ComponentContainer<Motion> &motion_container = registry.motions;

	for (uint i = 0; i < motion_container.components.size(); i++) {
		Motion& motion = motion_container.components[i];
		float halfScaleX = motion.scale.x / 2;
		float halfScaleY = getWorldYPosition(motion.scale.y) / 2;

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
	ComponentContainer<Hitbox> &hitboxes = registry.hitboxes;
	for (uint i = 0; i < hitboxes.components.size(); i++) {
		Entity entity_i = hitboxes.entities[i];
		for (uint j = i + 1; j < hitboxes.components.size(); j++) {
			Entity entity_j = hitboxes.entities[j];
			if (collides(entity_i, entity_j)) {
				// Collision detected
				// std::cout << "Collision detected between entities " << entity_i << " and " << entity_j << std::endl;
				collisions.push_back(std::make_pair(entity_i, entity_j));
				collisions.push_back(std::make_pair(entity_j, entity_i));
			}
		}
	}

	handleBoundsCheck();
};