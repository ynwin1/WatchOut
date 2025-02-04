#pragma once

// stlib
#include <vector>
#include <random>
#include <unordered_set>

// internal 
#include <render_system.hpp>
#include <physics_system.hpp>
#include <ai_system.hpp>
#include <sound_system.hpp>
#include <world_init.hpp>
#include <game_state_controller.hpp>
#include <game_save_manager.hpp>
#include <spawn_manager.hpp>

// Container for all our entities and game logic
class WorldSystem
{
public:
	WorldSystem(std::default_random_engine& rng);

	// starts the game
	void init(
		RenderSystem* renderer, 
		GLFWwindow* window, 
		Camera* camera, 
		PhysicsSystem* physics, 
		AISystem* ai, 
		SoundSystem* sound, 
		GameSaveManager* saveManager, 
		SpawnManager* spawnManager
	);

	// Releases all associated resources
	~WorldSystem();

	GameStateController gameStateController;

	std::unordered_map<std::string, float> spawn_delays;
	std::unordered_map<std::string, int> max_entities;
	std::unordered_map<std::string, float> next_spawns;

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	bool show_mesh;
	float countdown = 0.0f;

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;
	
	friend class GameStateController;

	// restart level
	void restart_game();
	void initText();
	void soundSetUp();

	// load game
	void load_game();
	void reloadText();

private:
	const float DIFFICULTY_INTERVAL = 45000.0f;
	const unsigned int MAX_TOTAL_ENEMIES = 100;
	const float SURVIVAL_BONUS_INTERVAL = 120000.0f;

	std::string DAMAGE_TRAP = "trap";
	std::string PHANTOM_TRAP = "phantom_trap";

	bool isTutorialNeeded = true;

	// GLFW Window handle
	GLFWwindow* window;
	RenderSystem* renderer;
	ParticleSystem* particles;
	PhysicsSystem* physics;
	AISystem* ai;
	Camera* camera;
	SoundSystem* sound;
	TrapsCounter trapsCounter;
	GameSaveManager* saveManager;
	SpawnManager* spawnManager;

	bool isWindowed = false;

	//Tutorial initialization
	float tutorialDelayTimer = 0.0f; 
    bool hasSwitchedToTutorial = false;

	std::unordered_set<std::string> encounteredEnemies;
	std::unordered_set<std::string> encounteredCollectibles;

	Entity playerEntity;

	std::vector<std::string> entity_types = {
		"boar",
		"barbarian",
		"archer",
		"bird",
		"wizard",
		"troll",
		"bomber",
		"heart",
		"collectible_trap"
	};

	const std::unordered_map<std::string, int> initial_max_entities = {
		{"boar", 0},
		{"barbarian", 0},
		{"archer", 0},
		{"bird", 2},
		{"wizard", 0},
		{"troll", 0},
		{"bomber", 0},
		{"heart", 2},
		{"collectible_trap", 2}
	};
	const std::unordered_map<std::string, float> initial_spawn_delays = {
		{"boar", 10000.0f},
		{"barbarian", 10000.0f},
		{"archer", 20000.0f},
		{"bird", 20000.0f},
		{"wizard", 20000.0f},
		{"troll", 30000.0f},
		{"bomber", 20000.0f},
		{"heart", 5000.0f},
		{"collectible_trap", 5000.0f}
	};

	using spawn_func = Entity(*)(vec2);
	const std::unordered_map<std::string, spawn_func> spawn_functions = {
        {"boar", createBoar},
        {"barbarian", createBarbarian},
        {"archer", createArcher},
        {"bird", createBird},
	    {"wizard", createWizard},
        {"troll", createTroll},
		{"bomber", createBomber},
        {"heart", createHeart},
		{"collectible_trap", createCollectibleTrap}
    };

	// Keeps track of what collisions have been handled recently.
	// Key uses entities cast to ints for comparisons.
	std::map<std::pair<int, int>, float> collisionCooldowns;

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_position);
	void on_mouse_button(int button, int action, int mod);

	// Title screen
	void createTitleScreen();
	void createTitleScreenTutorial();

	// Save game
	void save_game();


	// Actions performed for each step
	void spawn(float elapsed_ms);
	void spawn_particles(float elapsed_ms);
	void update_cooldown(float elapsed_ms);
	void handle_deaths(float elapsed_ms);
	void update_player_facing(Player& player, Motion& motion);
	void despawn_collectibles(float elapsed_ms);
	void handle_stamina(float elapsed_ms);
	vec2 get_spawn_location(const std::string& entity_type);
	void place_trap(vec3 trapPos, std::string type);
	void checkAndHandlePlayerDeath(Entity& entity);
	void trackFPS(float elapsed_ms);
	void updateGameTimer(float elapsed_ms);
	void updateTrapsCounterText();
	void updateInventoryItemText();
	void toggleMesh();
	void adjustSpawnSystem(float elapsed_ms);
	void resetSpawnSystem();
	void loadAndSaveHighScore(bool save);
	void on_window_focus(int focused);
	void destroyDamagings();
	void accelerateFireballs(float elapsed_ms);
	void despawnTraps(float elapsed_ms);
	void updateCollectedTimer(float elapsed_ms);
	void resetTrappedEntities();
	void handleEnemiesKilledInSpan(float elapsed_ms);
	void updateComboText();
	void updateScoreText();
	void handleSurvivalBonusPoints(float elapsed_ms);
	void updatePointLightPositions(float elapsed_ms);
	void updateTutorial(float elapsed_ms);
	void updateEnemyTutorial();
	void updateCollectibleTutorial();
	void updateHomingProjectiles(float elapsed_ms);
	void updateEquippedPosition();
	void updateMouseTexturePosition(vec2 mousePos);
	void equipItem(INVENTORY_ITEM item, bool wasCollected = false);
	void unEquipItem();
	INVENTORY_ITEM getNextInventoryItem(INVENTORY_ITEM currentItem);

	// Collision functions
	void entity_collectible_collision(Entity entity, Entity collectible);
	void entity_trap_collision(Entity entity, Entity trap, std::vector<Entity>& was_damaged);
	void entity_damaging_collision(Entity entity, Entity trap, std::vector<Entity>& was_damaged);
	void entity_obstacle_collision(Entity entity, Entity obstacle, std::vector<Entity>& was_damaged);
	void damaging_obstacle_collision(Entity entity);
	void processPlayerEnemyCollision(Entity player, Entity enemy, std::vector<Entity>& was_damaged);
	void handleEnemyCollision(Entity attacker, Entity target, std::vector<Entity>& was_damaged);
	void checkAndHandleEnemyDeath(Entity entity);
	void knock(Entity knocked, Entity knocker);
	void setCollisionCooldown(Entity damager, Entity victim);

	// Controls
	void allStateControls(int key, int action, int mod);
	void movementControls(int key, int action, int mod);
	void titleControls(int key, int action, int mod);
	void titleTutorialControls(int key, int action, int mod);
	void playingControls(int key, int action, int mod);
	void pauseControls(int key, int action, int mod);
	void gameOverControls(int key, int action, int mod);
	void helpControls(int key, int action, int mod);
	void tutorialControls(int key, int action, int mod);
	void enemyTutorialControls(int key, int action, int mod);
	void collectibleTutorialControls(int key, int action, int mod);
	void onTutorialClick();

	void handleSoundOnPauseHelp();

	void clearSaveText();

	// Help/Pause Menu functions
	Entity createHelpMenu(vec2 cameraPosition);
    void exitHelpMenu();
    Entity createPauseMenu(vec2 cameraPosition);
    void exitPauseMenu();
	Entity createTutorial(vec2 cameraPosition);
	void exitTutorial();
	Entity createBoarTutorial(vec2 cameraPosition);
	Entity createBirdTutorial(vec2 cameraPosition);
	Entity createWizardTutorial(vec2 cameraPosition);
	Entity createTrollTutorial(vec2 cameraPosition);
	Entity createArcherTutorial(vec2 cameraPosition);
	Entity createBarbarianTutorial(vec2 cameraPosition);
	Entity createBomberTutorial(vec2 cameraPosition);
	void exitEnemyTutorial();
	Entity createHeartTutorial(vec2 cameraPosition);
	Entity createTrapTutorial(vec2 cameraPosition);
	Entity createPhantomTrapTutorial(vec2 cameraPosition);
	Entity createBowTutorial(vec2 cameraPosition);
	Entity createBombTutorial(vec2 cameraPosition);
	void exitCollectibleTutorial();

	void leftMouseClickAction(vec3 mouseWorldPos);
	void shootArrow(vec3 mouseWorldPos);
	Entity shootProjectile(vec3 mouseWorldPos, PROJECTILE_TYPE type);
	Entity shootHomingArrow(Entity targetEntity, float angle);

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
    // number between 0..1
};
