#pragma once
#include "json.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include <render_system.hpp>
#include "camera.hpp"

class GameSaveManager {
public:
	using json = nlohmann::json;

	void init(RenderSystem* renderer, GLFWwindow* window, Camera* camera);

	// Save the game
	void save_game(std::unordered_map<std::string, std::pair<int, Entity>> trapCounter, std::unordered_map<std::string, float> spawn_delays, std::unordered_map<std::string, int> max_entities, std::unordered_map<std::string, float> next_spawns);

	// Load the game
	void load_game();
	
	// get spawn delays
	std::unordered_map<std::string, float> getSpawnDelays();
	// get max entities
	std::unordered_map<std::string, int> getMaxEntities();
	// get next spawns
	std::unordered_map<std::string, float> getNextSpawns();
  // load trap counter
	void loadTrapsCounter(std::unordered_map<std::string, std::pair<int, Entity>>& trapCounterWorld);

private:

	RenderSystem* renderer;
	GLFWwindow* window;
	Camera* camera;

	std::unordered_map<std::string, std::pair<int, Entity>> trapsCounter;

	// CONSTANTS
	// CONTAINERS
	std::string MOTIONS = "motions";
	std::string PLAYERS = "players";
	std::string DASHERS = "dashers";
	std::string ENEMIES = "enemies";
	std::string COLLISIONS = "collisions";
	std::string COOLDOWNS = "cooldowns";
	std::string COLLECTIBLES = "collectibles";
	std::string TRAPS = "traps";
	std::string DAMAGEDS = "damageds";
	std::string DAMAGINGS = "damagings";
	std::string DEATHTIMERS = "deathTimers";
	std::string KNOCKABLES = "knockables";
	std::string KNOCKERS = "knockers";
	std::string TRAPPABLES = "trappables";
	std::string HEALTHBARS = "healthBars";
	std::string ANIMATIONCONTROLLERS = "animationControllers";
	std::string STAMINABARS = "staminaBars";
	std::string STAMINAS = "staminas";
	std::string TEXTS = "texts";
	std::string JUMPERS = "jumpers";
	std::string PROJECTILES = "projectiles";
	std::string TARGETAREAS = "targetAreas";
	std::string RENDERREQUESTS = "renderRequests";
	std::string BACKGROUNDS = "backgrounds";
	std::string MIDGROUNDS = "midgrounds";
	std::string FOREGROUNDS = "foregrounds";
	std::string COLOURS = "colours";
	std::string MAPTILES = "mapTiles";
	std::string OBSTACLES = "obstacles";
	std::string MESHES = "meshPtrs";
	std::string COLLECTED = "collected";
	std::string MESHPTRS = "meshPtrs";
	std::string BOARS = "boars";
	std::string BARBARIANS = "barbarians";
	std::string ARCHERS = "archers";
	std::string BIRDS = "birds";
	std::string WIZARDS = "wizards";
	std::string TROLLS = "trolls";
	std::string HEARTS = "hearts";
	std::string COLLECTIBLETRAPS = "collectibleTraps";

	std::string GAMETIMER = "gameTimer";
	std::string GAMESCORE = "gameScore";
	std::string SPAWNDELAYS = "spawn_delays";
	std::string MAXENTITIES = "max_entities";
	std::string NEXTSPAWNS = "next_spawns";
	std::string TRAPCOUNTER = "trapsCounter";

	// Game Save file path
	std::string gameSaveFilePath = data_path() + "/save/game_save.json";
	// Map to store group of components for each entity
	std::map<int, std::map<std::string, json>> entityComponentGroups;
	std::unordered_map<std::string, float> spawnDelays;
	std::unordered_map<std::string, int> maxEntities;
	std::unordered_map<std::string, float> nextSpawns;

	// Serialization
	void serialize_containers(json& j, std::unordered_map<std::string, std::pair<int, Entity>> trapsCounter, std::unordered_map<std::string, float> spawn_delays, std::unordered_map<std::string, int> max_entities, std::unordered_map<std::string, float> next_spawns);

	template <typename Component>
	json serialize_container(const ComponentContainer<Component>& container);
	json serialize_mesh_container(const ComponentContainer<Mesh*>& container);

	nlohmann::json serialize_game_timer(const GameTimer& gameTimer);
	nlohmann::json serialize_game_score(const GameScore& gameScore);

	nlohmann::json serialize_spawn_delays(std::unordered_map<std::string, float> spawn_delays);
	nlohmann::json serialize_max_entities(std::unordered_map<std::string, int> max_entities);
	nlohmann::json serialize_next_spawns(std::unordered_map<std::string, float> next_spawns);
	nlohmann::json serialize_traps_counter(const std::unordered_map<std::string, std::pair<int, Entity>> trapCounter);


	template <typename Component>
	nlohmann::json serialize_component(const Component& component);

	// Deserialization
	void groupComponentsForEntities(const json& j);

	void deserialize_containers(const json& j);
	void createEntity(std::vector<std::string> componentNames, std::map<std::string, nlohmann::json> componentsMap);
	void deserialize_game_timer(const json& j);
	void deserialize_game_score(const json& j);
	void deserialize_spawn_delays(const json& j);
	void deserialize_max_entities(const json& j);
	void deserialize_next_spawns(const json& j);
  void deserialize_traps_counter(const json& j);

	void createPlayerDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createBoarDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createBarbarianDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createArcherDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createBirdDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createWizardDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTrollDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createHeartDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createCollectibleTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTreeDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createObstacleDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTargetAreaDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createDamagingsDeserialization(std::map<std::string, nlohmann::json> componentsMap);

	// helpers
	void handleDeathTimer(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleMotion(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleTrappable(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleEnemy(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleDasher(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleKnocker(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleCooldown(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);

	void handleBoar(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleBarbarian(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleArcher(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleBird(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleWizard(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
	void handleTroll(Entity& entity, std::map<std::string, nlohmann::json> componentsMap);
};