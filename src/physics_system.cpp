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
	// Z dimension checks
	if (a_position.z > b_position.z + ((b_dimension.z + a_dimension.z) / 2.0f))
		return false;
	if (a_position.z + ((a_dimension.z + b_dimension.z) / 2.0f) < b_position.z)
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

void PhysicsSystem::checkCollisions() 
{
	// Check for collisions between moving entities
	ComponentContainer<Hitbox> &hitboxes = registry.hitboxes;
	for (uint i = 0; i < hitboxes.components.size(); i++) {
		Entity entity_i = hitboxes.entities[i];
		for (uint j = i + 1; j < hitboxes.components.size(); j++) {
			Entity entity_j = hitboxes.entities[j];
			if (collides(entity_i, entity_j)) {
				// Collision detected
				collisions.push_back(std::make_pair(entity_i, entity_j));
				collisions.push_back(std::make_pair(entity_j, entity_i));
			}
		}
	}
}


void PhysicsSystem::updatePositions(float elapsed_ms)
{
	for (Entity entity : registry.motions.entities) {
		Motion& motion = registry.motions.get(entity);

		// Z-position of entity when it is on the ground
		float groundZ = getElevation(vec2(motion.position)) + motion.scale.y / 2;

		// Set player velocity
		if (registry.players.has(entity) && motion.position.z <= groundZ) {
			Player& player_comp = registry.players.get(entity);

			float player_speed = 0.5;
			if (!player_comp.isMoving) player_speed = 0;
			else if (player_comp.isRunning) player_speed *= 2;

			motion.velocity.x = (player_speed * player_comp.facing).x;
			motion.velocity.y = (player_speed * player_comp.facing).y;
		}

		// Update the entity's position based on its velocity and elapsed time
		motion.position.x += motion.velocity.x * elapsed_ms;
		motion.position.y += motion.velocity.y * elapsed_ms;
		motion.position.z += motion.velocity.z * elapsed_ms;

		// Apply gravity
		const float GRAVITATIONAL_CONSTANT = 0.01;
		if (motion.position.z > groundZ) {
			motion.velocity.z -= GRAVITATIONAL_CONSTANT * elapsed_ms;
		}

		// Hit the ground
		if (motion.position.z < groundZ) {
			motion.position.z = groundZ;
			motion.velocity.z = 0;
			if (registry.players.has(entity)) {
				registry.players.get(entity).isJumping = false;
			}
		}

		// Dashing overwrites normal movement
		if (registry.dashers.has(entity)) {
			Dash& dashing = registry.dashers.get(entity);
			if (dashing.isDashing) {
				dashing.dashTimer += elapsed_ms / 1000.0f; // Converting ms to seconds

				if (dashing.dashTimer < dashing.dashDuration) {
					// Interpolation factor
					float t = dashing.dashTimer / dashing.dashDuration;

					// Interpolate between start and target positions
					//player_motion.position is the target_position for the linear interpolation formula L(t)=(1−t)⋅A+t⋅B
					// L(t) = interpolated position, A = original position, B = target position, and t is the interpolation factor
					motion.position = vec3(glm::mix(dashing.dashStartPosition, dashing.dashTargetPosition, t), motion.position.z);
				}
				else {
					motion.position = vec3(dashing.dashTargetPosition, motion.position.z);
					dashing.isDashing = false; // Reset isDashing
				}
			}
		}
	}
}

void PhysicsSystem::step(float elapsed_ms)
{
	updatePositions(elapsed_ms);
	checkCollisions();
	handleBoundsCheck();
};