#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>


// PlayerComponents 
struct Player {
	unsigned int health = 100;
	unsigned int trapsCollected = 0;
  bool isRunning;     // Indicates if the player is currently running
  bool isJumping;     // Indicates if the player is currently jumping
	bool isRolling;     // Indicates if the player is currently rolling
};

//Dashing
struct Dash {
    bool isDashing;     // Indicates if the entity is currently dashing
	  glm::vec2 dashStartPosition; // Start position for dash
    glm::vec2 dashTargetPosition; // Target position for dash
    float dashTimer = 0.0f;       // Timer to track dash progress
	  float dashDuration = 0.2f;    // Duration of Dash
};

struct Enemy
{
	unsigned int health = 100;
	unsigned int damage = 10;
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
	unsigned int damage = 5.0;
};

// All data relevant to the shape and motion of entities
struct Motion {
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