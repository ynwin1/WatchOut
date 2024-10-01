#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>

// Unique Player Components 
struct Player {
    bool isRunning;     // Indicates if the player is currently running
    bool isJumping;     // Indicates if the player is currently jumping
    bool isDashing;     // Indicates if the player is currently dashing
	bool isRolling;     // Indicates if the player is currently rolling
};

// All data relevant to the shape and motion of entities
struct Motion 
{
	vec2 position = { 0, 0 }; // Represents the current position
	float angle = 0;
	vec2 velocity = { 0, 0 }; // Represents the speed and direction
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