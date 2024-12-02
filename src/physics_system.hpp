#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "sound_system.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void init(SoundSystem* sound);
	void step(float elapsed_ms);

	// Array to store collision pairs
	std::vector<std::pair<Entity, Entity>> collisions;

private:
	SoundSystem* sound;

	void updatePositions(float elapsed_ms);
	void checkCollisions();
	void handleBoundsCheck();
	void recoil_entities(Entity motion1, Entity motion2);
	void handle_mesh_collision(Entity entityM, Entity other_entity);
	void handle_obstacle_collision(Entity entityM, Entity obstacleM);
	bool meshCollides(Entity& mesh_entity, Entity& other_entity);
};

std::vector<vec3> boundingBoxVertices(Motion& motion);
bool polygonsCollide(const std::vector<vec2>& polygon1, const std::vector<vec2>& polygon2);

const float GRAVITATIONAL_CONSTANT = 0.01;
const float BOUNCE_FACTOR = 0.5f;
const float FRICTION_FACTOR = 0.95f;
