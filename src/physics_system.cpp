﻿#include "physics_system.hpp"
#include "world_init.hpp"
#include "render_system.hpp"
#include <iostream>

bool collides(const Entity& a, const Entity& b)
{
	Motion& motionA = registry.motions.get(a);
	Motion& motionB = registry.motions.get(b);
	// position represents the center of the entity
	vec3 a_position = motionA.position;
	vec3 b_position = motionB.position;
	// dimension represents the width and height of entity
	vec3 a_dimension = motionA.hitbox;
	vec3 b_dimension = motionB.hitbox;

	// If a's bottom is higher than b's top
	if (a_position.y > b_position.y + ((b_dimension.y + a_dimension.y) / 2.0f)) {
		return false;
	}
	// If a's top is lower than b's bottom
	if (a_position.y + ((a_dimension.y + b_dimension.y) / 2.0f) < b_position.y) {
		return false;
	}
	// If a's right is to the left of b's left
	if (a_position.x > b_position.x + ((b_dimension.x + a_dimension.x) / 2.0f)) {
		return false;
	}
	// Check if a's left is to the right of b's right
	if (a_position.x + ((a_dimension.x + b_dimension.x) / 2.0f) < b_position.x) {
		return false;
	}
		
	// Z dimension checks
	if (a_position.z > b_position.z + ((b_dimension.z + a_dimension.z) / 2.0f)) {
		return false;
	}
		
	if (a_position.z + ((a_dimension.z + b_dimension.z) / 2.0f) < b_position.z) {
		return false;
	}
		

	return true;
}

void PhysicsSystem::handleBoundsCheck() {
	ComponentContainer<Motion> &motion_container = registry.motions;

	for (uint i = 0; i < motion_container.components.size(); i++) {
		Motion& motion = motion_container.components[i];
		float halfScaleX = abs(motion.scale.x)/ 2;

		if (motion.position.x - halfScaleX < 0) {
			motion.position.x = halfScaleX;
		} 
		else if (motion.position.x + halfScaleX > world_size_x) {
			motion.position.x = world_size_x - halfScaleX;
		}

		if (motion.position.y < visualToWorldY(motion.scale.y)) {
			motion.position.y = visualToWorldY(motion.scale.y);
		} 
		else if (motion.position.y > world_size_y) {
			motion.position.y = world_size_y;
		}
	}
}

void PhysicsSystem::checkCollisions()
{
	// Check for collisions between moving entities
	ComponentContainer<Motion>& motions = registry.motions;
	for (uint i = 0; i < motions.components.size(); i++) {
		Entity entity_i = motions.entities[i];

		for (uint j = i + 1; j < motions.components.size(); j++) {
			Entity entity_j = motions.entities[j];

			// skip obstacle to obstacle collision
			if (registry.obstacles.has(entity_i) && registry.obstacles.has(entity_j)) continue;

			if (collides(entity_i, entity_j)) {
				if (registry.meshPtrs.has(entity_i)) {
					if (meshCollides(entity_i, entity_j)) {
						handle_mesh_collision(entity_i, entity_j);
					}
				}
				else if (registry.meshPtrs.has(entity_j)) {
					if (meshCollides(entity_j, entity_i)) {
						handle_mesh_collision(entity_j, entity_i);
					}
				}
				else {
					// Collision detected
					collisions.push_back(std::make_pair(entity_i, entity_j));
					collisions.push_back(std::make_pair(entity_j, entity_i));

					// Push each other
					if (motions.components[i].solid && motions.components[j].solid) {
						if (registry.obstacles.has(entity_i)) { //obstacle collision
							handle_obstacle_collision(entity_i, entity_j);
						}
						else if (registry.obstacles.has(entity_j)) {
							handle_obstacle_collision(entity_j, entity_i);
						}
						else {
							recoil_entities(entity_i, entity_j);
						}
					}
				}
			}
		}
	}
}

static vec3 tranformVertex(vec3 vertex, vec3 translation, float rotation, vec3 scaling)
{
	// Scaling
	vertex *= scaling;
	
	// Rotation
	vertex.x = vertex.x * cos(rotation) - vertex.z * sin(rotation);
	vertex.z = vertex.x * sin(rotation) + vertex.z * cos(rotation);

	// Translation
	vertex += translation;

	return vertex;
}

bool polygonsCollide(std::vector<vec2> polygon1, std::vector<vec2> polygon2) {
	// Check if two polygons are intersecting
	for (int i = 0; i < 2; i++) {
		std::vector<vec2> polygon = i == 0 ? polygon1 : polygon2;
		for (int i1 = 0; i1 < polygon.size(); i1++) {
			int i2 = (i1 + 1) % polygon.size();
			vec2 p1 = polygon[i1];
			vec2 p2 = polygon[i2];

			vec2 normal = { p2.y - p1.y, p1.x - p2.x };

			float minA = normal.x * polygon1[0].x + normal.y * polygon1[0].y;
			float maxA = minA;
			for (int j = 0; j < polygon1.size(); j++) {
				float projected = normal.x * polygon1[j].x + normal.y * polygon1[j].y;
				if (projected < minA) minA = projected;
				if (projected > maxA) maxA = projected;
			}

			float minB = normal.x * polygon2[0].x + normal.y * polygon2[0].y;
			float maxB = minB;
			for (int j = 0; j < polygon2.size(); j++) {
				float projected = normal.x * polygon2[j].x + normal.y * polygon2[j].y;
				if (projected < minB) minB = projected;
				if (projected > maxB) maxB = projected;
			}

			if (maxA < minB || maxB < minA) {
				return false;
			}
		}
	}
	return true;
}

bool PhysicsSystem::meshCollides(Entity& mesh_entity, Entity& other_entity) {
	Mesh& mesh = *(registry.meshPtrs.get(mesh_entity));
	Motion& mesh_motion = registry.motions.get(mesh_entity);
	Motion& other_motion = registry.motions.get(other_entity);
	// Polygon vertices
	std::vector<vec2> otherPolygon;
	float halfWidth = other_motion.hitbox.x / 2;
	float halfDepth = other_motion.hitbox.y / 2;
	float halfHeight = other_motion.hitbox.z / 2;
	vec2 horizontalDirection = normalize(vec2(mesh_motion.position) - vec2(other_motion.position));
	
	// Initialize with big numbers that will always be overwritten
	float minHorizontalPos = FLT_MAX;
	float maxHorizontalPos = -FLT_MAX;
	float minVerticalPos = FLT_MAX;
	float maxVerticalPos = -FLT_MAX;
	for (auto i : { -1, 1 }) {
		for (auto j : { -1, 1 }) {
			for (auto k : { -1, 1 }) {
				// Get vertex in world space
				vec3 vertex = vec3(halfWidth * i, halfDepth * j, halfHeight * k);
				vertex = tranformVertex(vertex, other_motion.position, other_motion.angle, vec3(1));

				// Get vertex along collision axis relative to the mesh
				vec2 horizontalVector = vec2(vertex) - vec2(mesh_motion.position);
				float verticalPos = vertex.z - mesh_motion.position.z;
				float horizonalPos = dot(horizontalVector, horizontalDirection);
				if (horizonalPos < minHorizontalPos) {
					minHorizontalPos = horizonalPos;
				}
				if (horizonalPos > maxHorizontalPos) {
					maxHorizontalPos = horizonalPos;
				}
				if (verticalPos < minVerticalPos) {
					minVerticalPos = verticalPos;
				}
				if (verticalPos > maxVerticalPos) {
					maxVerticalPos = verticalPos;
				}
			}
		}
	}
	otherPolygon.push_back({ minHorizontalPos, minVerticalPos });
	otherPolygon.push_back({ maxHorizontalPos, minVerticalPos });
	otherPolygon.push_back({ maxHorizontalPos, maxVerticalPos });
	otherPolygon.push_back({ minHorizontalPos, maxVerticalPos });

	std::vector<uint16_t>& faces = mesh.vertex_indices;
	for (int i = 0; i < faces.size(); i += 3) {
		std::vector<vec2> meshPolygon;

		vec3 collisionVertex;
		// Get the vertices of the face
		for (int j = 0; j < 3; j++) {
			vec2 v = mesh.vertices[faces[i + j]].position;
			vec3 vertex = { v.x, 0, -v.y };
			vec3 scaling = { mesh_motion.scale.x, 0, mesh_motion.scale.y / zConversionFactor };
			vec3 translation = vec3(0);
			vertex = tranformVertex(vertex, translation, mesh_motion.angle, scaling);
			meshPolygon.push_back({ vertex.x, vertex.z });
		}

		// Check if the transformed vertices are within the bounding box
		if (polygonsCollide(meshPolygon, otherPolygon)) {
			return true;
		}
	}
	return false;
}


void PhysicsSystem::updatePositions(float elapsed_ms)
{
	for (Entity entity : registry.motions.entities) {
		Motion& motion = registry.motions.get(entity);

		// Z-position of entity when it is on the ground
		float groundZ = getElevation(vec2(motion.position)) + motion.hitbox.z / 2;

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
		if (motion.position.z > groundZ) {
			motion.velocity.z -= GRAVITATIONAL_CONSTANT * elapsed_ms;
		}

		// Hit the ground
		if (motion.position.z < groundZ) {
			motion.position.z = groundZ;
			motion.velocity.z = 0;
			if (registry.jumpers.has(entity)) {
				Jumper& jumper = registry.jumpers.get(entity);
				if (registry.players.has(entity) && !registry.players.get(entity).tryingToJump) {
					jumper.isJumping = false;
				}
				else {
					motion.velocity.z = jumper.speed;
				}
			}
			else if (registry.projectiles.has(entity)) {
				motion.velocity.x = 0;
				motion.velocity.y = 0;
				if (registry.damagings.has(entity)) {
					registry.damagings.remove(entity);
				}
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

float calculate_x_overlap(Entity entity1, Entity entity2) {
	Motion& motion1 = registry.motions.get(entity1);
	Motion& motion2 = registry.motions.get(entity2);

	float x1_half_scale = motion1.hitbox.x / 2;
	float x2_half_scale = motion2.hitbox.x / 2;

	// print x of entity1 and 2
	// std::cout << "x1: " << motion1.position.x << " x2: " << motion2.position.x << std::endl;

	// Determine the edges of the hitboxes for x
	float left1 = motion1.position.x - x1_half_scale;
	float right1 = motion1.position.x + x1_half_scale;
	float left2 = motion2.position.x - x2_half_scale;
	float right2 = motion2.position.x + x2_half_scale;

	// Calculate x overlap
	return max(0.f, min(right1, right2) - max(left1, left2));
}

float calculate_y_overlap(Entity entity1, Entity entity2) {
	Motion& motion1 = registry.motions.get(entity1);
	Motion& motion2 = registry.motions.get(entity2);

	float y1_half_scale = motion1.hitbox.y / 2;
	float y2_half_scale = motion2.hitbox.y / 2;

	// Determine the edges of the hitboxes for y
	float top1 = motion1.position.y - y1_half_scale;
	float bottom1 = motion1.position.y + y1_half_scale;
	float top2 = motion2.position.y - y2_half_scale;
	float bottom2 = motion2.position.y + y2_half_scale;

	// Calculate y overlap
	return max(0.f, min(bottom1, bottom2) - max(top1, top2));
}

void PhysicsSystem::handle_mesh_collision(Entity mesh, Entity entity) {

	float PUSH_BACK = 5.f;

	Motion& meshMotion = registry.motions.get(mesh);
	Motion& entityMotion = registry.motions.get(entity);

	// direction of the collision
	float x_direction = meshMotion.position.x < entityMotion.position.x ? 1 : -1;
	float y_direction = meshMotion.position.y < entityMotion.position.y ? 1 : -1;

	entityMotion.position.x += x_direction * PUSH_BACK;
	entityMotion.position.y += y_direction * PUSH_BACK;
	entityMotion.velocity.z = 0;
}

void PhysicsSystem::handle_obstacle_collision(Entity obstacle, Entity entity) {
	// Calculate x overlap
	float x_overlap = calculate_x_overlap(obstacle, entity);
	// Calculate y overlap
	float y_overlap = calculate_y_overlap(obstacle, entity);

	// print mesh collision position
	// std::cout << "mesh collision position: " << meshCollisionPosition.x << " " << meshCollisionPosition.y << " " << meshCollisionPosition.z << std::endl;

	// print the overlap
	// std::cout << "x overlap: " << x_overlap << " y overlap: " << y_overlap << std::endl;

	Motion& obstacleM = registry.motions.get(obstacle);
	Motion& entityM = registry.motions.get(entity);

	// Calculate the direction of the collision
	float x_direction = obstacleM.position.x < entityM.position.x ? 1 : -1;
	float y_direction = obstacleM.position.y < entityM.position.y ? 1 : -1;

	// print mesh position
	// std::cout << "obstacle position: " << obstacleM.position.x << " " << obstacleM.position.y << " " << obstacleM.position.z << std::endl;

	// print entity position
	// std::cout << "entity position before: " << entityM.position.x << " " << entityM.position.y << " " << entityM.position.z << std::endl;
	if (y_overlap < x_overlap) {
		entityM.position.y += y_direction * y_overlap;
	}
	else {
		entityM.position.x += x_direction * x_overlap;
	}

	// print entity position
	// std::cout << "entity position after: " << entityM.position.x << " " << entityM.position.y << entityM.position.z << std::endl;

	if(registry.dashers.has(entity)) {
		registry.dashers.get(entity).isDashing = false;
	}
}

void PhysicsSystem::recoil_entities(Entity entity1, Entity entity2) {
	// Calculate x overlap
	float x_overlap = calculate_x_overlap(entity1, entity2);
	// Calculate y overlap
	float y_overlap = calculate_y_overlap(entity1, entity2);

	Motion& motion1 = registry.motions.get(entity1);
	Motion& motion2 = registry.motions.get(entity2);

	// Calculate the direction of the collision
	float x_direction = motion1.position.x < motion2.position.x ? -1 : 1;
	float y_direction = motion1.position.y < motion2.position.y ? -1 : 1;

	// Apply the recoil (direction * magnitude)
	const float RECOIL_STRENGTH = 0.25;
	if (y_overlap < x_overlap) {
		motion1.position.y += y_direction * y_overlap * RECOIL_STRENGTH;
		motion2.position.y -= y_direction * y_overlap * RECOIL_STRENGTH;
	}
	else {
		motion1.position.x += x_direction * x_overlap * RECOIL_STRENGTH;
		motion2.position.x -= x_direction * x_overlap * RECOIL_STRENGTH;
	}
}

void PhysicsSystem::step(float elapsed_ms)
{
	updatePositions(elapsed_ms);
	checkCollisions();
	handleBoundsCheck();
};