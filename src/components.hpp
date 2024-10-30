#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>


// PlayerComponents 
struct Player {
	unsigned int health = 100;
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

//Stamina
struct Stamina {
	float stamina = 100;
	float max_stamina = 100;     
    float stamina_loss_rate = 50;
	float stamina_recovery_rate = 10;
	float timer = 3000;

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

struct Damaging {
	unsigned int damage = 10;
};

struct Projectile {
	float sticksInGround = 3000; // ms it lasts on the ground
};

struct HealthBar {
	Entity meshEntity;
	float width;
	float height;
	HealthBar(Entity& meshEntity) { this->meshEntity = meshEntity; };
};

struct StaminaBar {
	Entity meshEntity;
	float width;
	float height;
	StaminaBar(Entity& meshEntity) { this->meshEntity = meshEntity; };
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
	vec2 scale = { 10, 10 };	// only for rendering

	// Hitbox
	vec3 hitbox = { 0, 0, 0 };
	float solid = false;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
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

struct TrapsCounter {
	int count = 0;
	Entity textEntity;
	void reset() {
		count = 0;
	}
};

struct GameTimer {
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	float ms = 0;
	Entity textEntity;
	void update(float elapsedTime) {
		ms += elapsedTime;
		if(ms >= 1000.f) {
			ms -= 1000;
        	seconds += 1;
    	}
    	if(seconds >= 60) {
        	seconds -= 60;
        	minutes += 1;
    	}
    	if(minutes >= 60) {
        	minutes -= 60;
        	hours += 1;
    	}
	}
	void reset() {
		hours = 0;
		minutes = 0;
		seconds = 0;
	}
};

struct Text {
	std::string value;
	vec2 position = { 0, 0 };
	float scale = 1.0f;
};

struct TextChar {
    unsigned int textureID;  // ID handle of the glyph texture
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

struct FPSTracker {
	int fps = 0;
	int counter = 0;
	float elapsedTime = 0;
	Entity textEntity;
	bool toggled = false;
	void update(float elapsed_ms) {
		elapsedTime += elapsed_ms;
		counter += 1;

    	if(elapsedTime >= 1000) {
        	fps = counter;
        	counter = 0;
        	elapsedTime = 0;
    	}
	}
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
struct Archer {
	float drawArrowTime = 0;
	bool aiming = false;
};

// Collectible types
struct Heart { unsigned int health = 20; };
struct CollectibleTrap {};