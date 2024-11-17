#pragma once
#include "common.hpp"
#include <vector>


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
	int health = 100;
	int maxHealth = 100;
	int damage = 10;
	std::string type;
	unsigned int cooldown = 0;
	float pathfindTime = 0;
};

struct Trappable {
	bool isTrapped = false;
	float originalSpeed = 0;
};

struct Damaging {
	std::string type = "arrow"; // default type
	unsigned int damage = 10;
};

struct Projectile {
	float sticksInGround = 3000; // ms it lasts on the ground
};

struct HealthBar {
	Entity meshEntity;
	Entity frameEntity;
	Entity textEntity;
	float width;
	float height;
	HealthBar(Entity& meshEntity, Entity& frameEntity) { 
		this->meshEntity = meshEntity; 
		this->frameEntity = frameEntity; 
	}
};

struct StaminaBar {
	Entity meshEntity;
	Entity frameEntity;
	Entity textEntity;
	float width;
	float height;
	StaminaBar(Entity& meshEntity, Entity& frameEntity) { 
		this->meshEntity = meshEntity; 
		this->frameEntity = frameEntity; 
	};
};

// Collectible Component
struct Collectible
{
	float duration = 5000.f; // 5 seconds until it disappears
	float timer = duration; 
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
};

struct Collected {
	float duration = 2500;
};

// Trap Component
struct Trap
{
	// fixed position and scale once set
	vec2 position = { 0, 0 };
	vec2 scale = { 3, 3 };
	unsigned int damage = 15.0;
	float duration = 10000;
	float slowFactor = 0.1f;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec3 position = { 0, 0, 0 };
	float angle = 0;
	vec3 velocity = { 0, 0, 0 };
	float speed = 0;			// max voluntary speed
	vec2 scale = { 10, 10 };	// only for rendering
	vec2 facing = { 0, 0 };		// direction the entity is facing

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

struct Knockable
{

};

struct TrapsCounter {
	int count = 0;
	Entity textEntity;
	void reset() {
		count = 0;
	}
};

struct MapTile {
	vec2 position;
	vec2 scale;
};

struct Obstacle {

};

struct TargetArea {
};

struct GameTimer {
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	float ms = 0;
	float elapsed = 0;
	Entity textEntity;
	void update(float elapsedTime) {
		ms += elapsedTime;
		elapsed += elapsedTime;
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
		ms = 0;
		elapsed = 0;
		hours = 0;
		minutes = 0;
		seconds = 0;
	}
};

struct Text {
	std::string value;
	vec2 position = { 0, 0 };
	vec3 colour = {1.0f, 1.0f, 1.0f};
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

struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Entity can jump
struct Jumper
{
	float speed = 0;
	bool isJumping = false;
};

// Enemy types
struct Boar {
	float cooldownTimer = 0;        
    float prepareTimer = 0;         
    float chargeTimer = 0;          
    bool preparing = false;         
    bool charging = false;          

    vec2 chargeDirection = vec2(0);
};
struct Barbarian {};
struct Archer {
	float drawArrowTime = 0;
	bool aiming = false;
};
struct Bird {
	float swarmSpeed = 0.3f;
	float swoopSpeed = 0.2f;
	bool isSwooping = false;
	float swoopTimer = 500;
	vec2 swoopDirection = {0,0};
	float originalZ = 480;
	float swoopCooldown = 2000;
};

enum WizardState { Moving, Aiming, Preparing, Shooting };
struct Wizard {
	WizardState state = WizardState::Moving;
	float shoot_cooldown = 0;
	float prepareLightningTime = 0;
	
	vec3 locked_target = vec3(0, 0, 0);
};

struct Troll {};

// Collectible types
struct Heart { unsigned int health = 20; };
struct CollectibleTrap {};

struct PauseMenuComponent {};
struct HelpMenuComponent {};