#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>

// Player Component
struct Player 
{
	float health = 100.0f;
};

struct Enemy
{
	float health = 100.0f;
};

// Collectible Component
struct Collectible
{
	// fixed position and scale once set
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
};

// Trap Component
struct Trap
{
	// fixed position and scale once set
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
	float damage = 5.0f;
};

// All data relevant to the shape and motion of entities
struct Motion 
{
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Structure to store hitbox information
struct Hitbox
{
	vec2 position = { 0, 0 };
	vec2 dimension = { 0, 0 };
};