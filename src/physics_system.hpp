#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms);

	// Array to store collision pairs
	std::vector<std::pair<Entity, Entity>> collisions;

private:
	void updatePositions(float elapsed_ms);
	void checkCollisions();
	void handleBoundsCheck();
	void recoil_entities(Entity motion1, Entity motion2);
	void handle_obstacle_collision(Entity entityM, Entity obstacleM);
	bool meshCollides(Entity& mesh_entity, Entity& other_entity);
};

const float GRAVITATIONAL_CONSTANT = 0.01;
