#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>

// Player Component
struct Player 
{

};

// Collectible Component
struct Collectible
{
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
	bool collected = false;
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