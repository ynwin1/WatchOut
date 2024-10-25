#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>


// PlayerComponents 
struct Player {
	unsigned int health = 100;
	unsigned int trapsCollected = 0;
	bool isRunning;     // Indicates if the player is currently running
	bool isRolling;     // Indicates if the player is currently rolling
	bool goingUp;		// Key for going up is held down
	bool goingDown;		// Key for going down is held down
	bool goingLeft;		// Key for going left is held down
	bool goingRight;	// Key for going right is held down
	bool tryingToJump;	// Key for jumping is held down
	bool isMoving;		// Indicates if any movement keys are pressed
	vec2 facing;		// Direction the player is facing
};

//Dashing
struct Dash {
    bool isDashing;				// Indicates if the entity is currently dashing
	vec2 dashStartPosition;		// Start position for dash
    vec2 dashTargetPosition;	// Target position for dash
    float dashTimer = 0.0f;     // Timer to track dash progress
	float dashDuration = 0.2f;  // Duration of Dash
};

struct Enemy
{
	unsigned int health = 100;
	unsigned int damage = 10;
	std::string type;
	unsigned int cooldown = 0;
	float speed = 0;
};

struct HealthBar {
	Entity meshEntity;
	float width;
	float height;
	HealthBar(Entity& meshEntity) { this->meshEntity = meshEntity; };
};

// Collectible Component
struct Collectible
{
	float timer = 5000.f; // 5 seconds until it disappears
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
};

// Trap Component
struct Trap
{
	// fixed position and scale once set
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
	unsigned int damage = 15.0;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec3 position = { 0, 0, 0 };
	float angle = 0;
	vec3 velocity = { 0, 0, 0 };
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
	vec3 dimension = { 0, 0, 0 };
};

// Collision Cooldown
struct Cooldown
{
	float remaining = 0.0f;
};


// Entity was damaged recently
struct Damaged
{
	float timer = 400;
};

// DeathTimer Component
struct DeathTimer
{
	float timer = 3000;
};

// Entity can jump
struct Jumper
{
	float speed = 0;
	bool isJumping = false;
};

// Enemy types
struct Boar {};
struct Barbarian {};
struct Archer {};

// Collectible types
struct Heart { unsigned int health = 20; };
struct CollectibleTrap {};