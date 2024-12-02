#include "game_save_manager.hpp"
#include "tiny_ecs_registry.hpp"
#include "json.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "world_init.hpp"
#include "animation_system.hpp"
#include "world_system.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void GameSaveManager::init(RenderSystem* renderer, GLFWwindow* window, Camera* camera) {
	this->renderer = renderer;
	this->window = window;
	this->camera = camera;
}

// Serialize the game state to a JSON file
void GameSaveManager::save_game(std::unordered_map<std::string, std::pair<int, Entity>> trapsCounter, std::unordered_map<std::string, float> spawn_delays, std::unordered_map<std::string, int> max_entities, std::unordered_map<std::string, float> next_spawns) {
	json j;

	serialize_containers(j, trapsCounter, spawn_delays, max_entities, next_spawns);

	std::ofstream file(gameSaveFilePath);
	if (file.is_open()) {
		file << j.dump(4);
		file.close();
	}
	else {
		std::cout << "Unable to open file to save game" << std::endl;
	}
}

void GameSaveManager::serialize_containers(json& j, std::unordered_map<std::string, std::pair<int, Entity>> trapsCounter, std::unordered_map<std::string, float> spawn_delays, std::unordered_map<std::string, int> max_entities, std::unordered_map<std::string, float> next_spawns) {
	j[GAMETIMER] = serialize_game_timer(registry.gameTimer);
	j[GAMESCORE] = serialize_game_score(registry.gameScore);
	j[TRAPCOUNTER] = serialize_traps_counter(trapsCounter);
	j[SPAWNDELAYS] = serialize_spawn_delays(spawn_delays);
	j[MAXENTITIES] = serialize_max_entities(max_entities);
	j[NEXTSPAWNS] = serialize_next_spawns(next_spawns);

	// Serialize all component containers
	j[PLAYERS] = serialize_container<Player>(registry.players);
	j[DASHERS] = serialize_container<Dash>(registry.dashers);
	j[ENEMIES] = serialize_container<Enemy>(registry.enemies);
	j[MOTIONS] = serialize_container<Motion>(registry.motions);
	j[COLLISIONS] = serialize_container<Collision>(registry.collisions);
	j[COOLDOWNS] = serialize_container<Cooldown>(registry.cooldowns);
	j[COLLECTIBLES] = serialize_container<Collectible>(registry.collectibles);
	j[TRAPS] = serialize_container<Trap>(registry.traps);
	j[DAMAGEDS] = serialize_container<Damaged>(registry.damageds);
	j[DAMAGINGS] = serialize_container<Damaging>(registry.damagings);
	j[DEATHTIMERS] = serialize_container<DeathTimer>(registry.deathTimers);
	j[KNOCKABLES] = serialize_container<Knockable>(registry.knockables);
	j[KNOCKERS] = serialize_container<Knocker>(registry.knockers);
	j[TRAPPABLES] = serialize_container<Trappable>(registry.trappables);
	j[HEALTHBARS] = serialize_container<HealthBar>(registry.healthBars);
	j[ANIMATIONCONTROLLERS] = serialize_container<AnimationController>(registry.animationControllers);
	j[STAMINABARS] = serialize_container<StaminaBar>(registry.staminaBars);
	j[STAMINAS] = serialize_container<Stamina>(registry.staminas);
	j[TEXTS] = serialize_container<Text>(registry.texts);
	j[JUMPERS] = serialize_container<Jumper>(registry.jumpers);
	j[MAPTILES] = serialize_container<MapTile>(registry.mapTiles);
	j[OBSTACLES] = serialize_container<Obstacle>(registry.obstacles);
	j[PROJECTILES] = serialize_container<Projectile>(registry.projectiles);
	j[MESHPTRS] = serialize_mesh_container(registry.meshPtrs); // SPECIAL CASE
	j[TARGETAREAS] = serialize_container<TargetArea>(registry.targetAreas);
	j[COLLECTED] = serialize_container<Collected>(registry.collected);
	j[RENDERREQUESTS] = serialize_container<RenderRequest>(registry.renderRequests);
	j[BACKGROUNDS] = serialize_container<Background>(registry.backgrounds);
	j[MIDGROUNDS] = serialize_container<Midground>(registry.midgrounds);
	j[FOREGROUNDS] = serialize_container<Foreground>(registry.foregrounds);
	j[COLOURS] = serialize_container<vec4>(registry.colours);
	j[BOARS] = serialize_container<Boar>(registry.boars);
	j[BARBARIANS] = serialize_container<Barbarian>(registry.barbarians);
	j[ARCHERS] = serialize_container<Archer>(registry.archers);
	j[BIRDS] = serialize_container<Bird>(registry.birds);
	j[WIZARDS] = serialize_container<Wizard>(registry.wizards);
	j[TROLLS] = serialize_container<Troll>(registry.trolls);
	j[HEARTS] = serialize_container<Heart>(registry.hearts);
	j[COLLECTIBLETRAPS] = serialize_container<CollectibleTrap>(registry.collectibleTraps);
}

template <typename Component>
nlohmann::json GameSaveManager::serialize_container(const ComponentContainer<Component>& container) {
	json j;
	json entities = json::array();
	json components = json::array();

	// save IDs of entities
	for (const auto& entity : container.entities) {
		entities.push_back(entity.getId());
	}

	// save components of entities
	for (const auto& component : container.components) {
		components.push_back(serialize_component(component));
	}

	j["entities"] = entities;
	j["components"] = components;

	return j;
}

nlohmann::json GameSaveManager::serialize_mesh_container(const ComponentContainer<Mesh*>& container) {
	json j;
	json entities = json::array();
	json components = json::array();

	// save IDs of entities
	for (const auto& entity : container.entities) {
		entities.push_back(entity.getId());
	}

	// save components of entities
	for (const auto& component : container.components) {
		json comp;
		components.push_back(comp);
	}

	j["entities"] = entities;
	j["components"] = components;

	return j;
}

// SERIALIZING COMPONENTS
nlohmann::json GameSaveManager::serialize_game_timer(const GameTimer& gameTimer) {
	nlohmann::json j;
	j["hour"] = gameTimer.hours;
	j["minute"] = gameTimer.minutes;
	j["second"] = gameTimer.seconds;
	j["ms"] = gameTimer.ms;
	j["elapsed"] = gameTimer.elapsed;
	j["textEntity"] = gameTimer.textEntity.getId();
	return j;
}

nlohmann::json GameSaveManager::serialize_game_score(const GameScore& gameScore) {
	nlohmann::json j;
	j["highScoreHours"] = gameScore.highScoreHours;
	j["highScoreMinutes"] = gameScore.highScoreMinutes;
	j["highScoreSeconds"] = gameScore.highScoreSeconds;
	return j;
}

nlohmann::json GameSaveManager::serialize_spawn_delays(std::unordered_map<std::string, float> spawn_delays) {
	nlohmann::json j;
	for (const auto& delay : spawn_delays) {
		j[delay.first] = delay.second;
	}
	return j;
}

nlohmann::json GameSaveManager::serialize_max_entities(std::unordered_map<std::string, int> max_entities) {
	nlohmann::json j;
	for (const auto& max : max_entities) {
		j[max.first] = max.second;
	}
	return j;
}

nlohmann::json GameSaveManager::serialize_next_spawns(std::unordered_map<std::string, float> next_spawns) {
	nlohmann::json j;
	for (const auto& next : next_spawns) {
		j[next.first] = next.second;
  }
	return j;
}

nlohmann::json GameSaveManager::serialize_traps_counter(std::unordered_map<std::string, std::pair<int, Entity>> trapsCounter) {
	nlohmann::json j;
	for (const auto& trap : trapsCounter) {
		nlohmann::json trapData;
		trapData["count"] = trap.second.first;
		trapData["textEntity"] = trap.second.second.getId();
		j[trap.first] = trapData;
	}
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Player>(const Player& player) {
	nlohmann::json j;
	j["health"] = player.health;
	j["isRunning"] = player.isRunning;
	j["isRolling"] = player.isRolling;
	j["goingUp"] = player.goingUp;
	j["goingDown"] = player.goingDown;
	j["goingLeft"] = player.goingLeft;
	j["goingRight"] = player.goingRight;
	j["tryingToJump"] = player.tryingToJump;
	j["isMoving"] = player.isMoving;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Dash>(const Dash& dash) {
	nlohmann::json j;
	j["isDashing"] = dash.isDashing;
	j["dashStartPosition"] = { dash.dashStartPosition.x, dash.dashStartPosition.y };
	j["dashTargetPosition"] = { dash.dashTargetPosition.x, dash.dashTargetPosition.y };
	j["dashTimer"] = dash.dashTimer;
	j["dashDuration"] = dash.dashDuration;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Enemy>(const Enemy& enemy) {
	nlohmann::json j;
	j["health"] = enemy.health;
	j["maxHealth"] = enemy.maxHealth;
	j["damage"] = enemy.damage;
	j["type"] = enemy.type;
	j["cooldown"] = enemy.cooldown;
	j["pathfindTime"] = enemy.pathfindTime;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Motion>(const Motion& motion) {
	nlohmann::json j;
	j["position"] = { motion.position.x, motion.position.y, motion.position.z };
	j["angle"] = motion.angle;
	j["velocity"] = { motion.velocity.x, motion.velocity.y, motion.velocity.z };
	j["speed"] = motion.speed;
	j["scale"] = { motion.scale.x, motion.scale.y };
	j["facing"] = { motion.facing.x, motion.facing.y };
	j["hitbox"] = { motion.hitbox.x, motion.hitbox.y, motion.hitbox.z };
	j["gravity"] = motion.gravity;
	j["solid"] = motion.solid;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Collision>(const Collision& collision) {
	nlohmann::json j;
	j["other"] = collision.other.getId();
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Cooldown>(const Cooldown& cooldown) {
	nlohmann::json j;
	j["remaining"] = cooldown.remaining;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Collectible>(const Collectible& collectible) {
	nlohmann::json j;
	j["duration"] = collectible.duration;
	j["timer"] = collectible.timer;
	j["position"] = { collectible.position.x, collectible.position.y };
	j["scale"] = { collectible.scale.x, collectible.scale.y };
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Trap>(const Trap& trap) {
	nlohmann::json j;
	j["position"] = { trap.position.x, trap.position.y };
	j["scale"] = { trap.scale.x, trap.scale.y };
	j["damage"] = trap.damage;
	j["duration"] = trap.duration;
	j["slowFactor"] = trap.slowFactor;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Damaged>(const Damaged& damaged) {
	nlohmann::json j;
	j["timer"] = damaged.timer;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Damaging>(const Damaging& damaging) {
	nlohmann::json j;
	j["type"] = damaging.type;
	j["damage"] = damaging.damage;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<DeathTimer>(const DeathTimer& deathTimer) {
	nlohmann::json j;
	j["timer"] = deathTimer.timer;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Knockable>(const Knockable& knockable) {
	nlohmann::json j;
	j["knocked"] = knockable.knocked;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Knocker>(const Knocker& knocker) {
	nlohmann::json j;
	j["strength"] = knocker.strength;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Trappable>(const Trappable& trappable) {
	nlohmann::json j;
	j["isTrapped"] = trappable.isTrapped;
	j["originalSpeed"] = trappable.originalSpeed;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<HealthBar>(const HealthBar& healthBar) {
	nlohmann::json j;
	j["meshEntity"] = healthBar.meshEntity.getId();
	j["frameEntity"] = healthBar.frameEntity.getId();
	j["textEntity"] = healthBar.textEntity.getId();
	j["width"] = healthBar.width;
	j["height"] = healthBar.height;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<AnimationController>(const AnimationController& animationController) {
	nlohmann::json j;
	j["currentState"] = animationController.currentState;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<StaminaBar>(const StaminaBar& staminaBar) {
	nlohmann::json j;
	j["meshEntity"] = staminaBar.meshEntity.getId();
	j["frameEntity"] = staminaBar.frameEntity.getId();
	j["textEntity"] = staminaBar.textEntity.getId();
	j["width"] = staminaBar.width;
	j["height"] = staminaBar.height;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Stamina>(const Stamina& stamina) {
	nlohmann::json j;
	j["stamina"] = stamina.stamina;
	j["max_stamina"] = stamina.max_stamina;
	j["stamina_loss_rate"] = stamina.stamina_loss_rate;
	j["stamina_recovery_rate"] = stamina.stamina_recovery_rate;
	j["timer"] = stamina.timer;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Text>(const Text& text) {
	nlohmann::json j;
	j["value"] = text.value;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Jumper>(const Jumper& jumper) {
	nlohmann::json j;
	j["speed"] = jumper.speed;
	j["isJumping"] = jumper.isJumping;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<MapTile>(const MapTile& mapTile) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Obstacle>(const Obstacle& obstacle) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Projectile>(const Projectile& projectile) {
	nlohmann::json j;
	j["sticksInGround"] = projectile.sticksInGround;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<TargetArea>(const TargetArea& targetArea) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Collected>(const Collected& collected) {
	nlohmann::json j;
	j["duration"] = collected.duration;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<RenderRequest>(const RenderRequest& renderRequest) {
	nlohmann::json j;
	j["used_texture"] = (int)renderRequest.used_texture;
	j["used_effect"] = (int)renderRequest.used_effect;
	j["used_geometry"] = (int)renderRequest.used_geometry;
	j["primitive_type"] = (int)renderRequest.primitive_type;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Background>(const Background& background) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Midground>(const Midground& midground) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Foreground>(const Foreground& foreground) {
	nlohmann::json j;
	j["position"] = { foreground.position.x, foreground.position.y };
	j["scale"] = { foreground.scale.x, foreground.scale.y };
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<vec4>(const vec4& colour) {
	nlohmann::json j;
	j["r"] = colour.r;
	j["g"] = colour.g;
	j["b"] = colour.b;
	j["a"] = colour.a;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Boar>(const Boar& boar) {
	nlohmann::json j;
	j["cooldownTimer"] = boar.cooldownTimer;
	j["prepareTimer"] = boar.prepareTimer;
	j["chargeTimer"] = boar.chargeTimer;
	j["preparing"] = boar.preparing;
	j["charging"] = boar.charging;
	j["chargeDirection"] = { boar.chargeDirection.x, boar.chargeDirection.y };
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Barbarian>(const Barbarian& barbarian) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Archer>(const Archer& archer) {
	nlohmann::json j;
	j["drawArrowTime"] = archer.drawArrowTime;
	j["aiming"] = archer.aiming;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Bird>(const Bird& bird) {
	nlohmann::json j;
	j["swarmSpeed"] = bird.swarmSpeed;
	j["swoopSpeed"] = bird.swoopSpeed;
	j["isSwooping"] = bird.isSwooping;
	j["swoopTimer"] = bird.swoopTimer;
	j["swoopDirection"] = { bird.swoopDirection.x, bird.swoopDirection.y };
	j["originalZ"] = bird.originalZ;
	j["swoopCooldown"] = bird.swoopCooldown;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Wizard>(const Wizard& wizard) {
	nlohmann::json j;
	j["state"] = wizard.state;
	j["shoot_cooldown"] = wizard.shoot_cooldown;
	j["prepareLightningTime"] = wizard.prepareLightningTime;
	j["locked_target"] = { wizard.locked_target.x, wizard.locked_target.y, wizard.locked_target.z };
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Troll>(const Troll& troll) {
	nlohmann::json j;
	j["desiredAngle"] = troll.desiredAngle;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<Heart>(const Heart& heart) {
	nlohmann::json j;
	j["health"] = heart.health;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<CollectibleTrap>(const CollectibleTrap& collectibleTrap) {
	nlohmann::json j;
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<GameTimer>(const GameTimer& gameTimer) {
	nlohmann::json j;
	j["hour"] = gameTimer.hours;
	j["minute"] = gameTimer.minutes;
	j["second"] = gameTimer.seconds;
	j["ms"] = gameTimer.ms;
	j["textEntity"] = gameTimer.textEntity.getId();
	return j;
}

template<>
nlohmann::json GameSaveManager::serialize_component<GameScore>(const GameScore& gameScore) {
	nlohmann::json j;
	j["highScoreHours"] = gameScore.highScoreHours;
	j["highScoreMinutes"] = gameScore.highScoreMinutes;
	j["highScoreSeconds"] = gameScore.highScoreSeconds;
	return j;
}

/*
%%%% DESERIALIZATION %%%%

Components are grouped by entity ID. A map is used to store this information.

The key of the map is an integer representing the entity ID.
The value of the map is another map with the key being the component name and the value being the component data.

Eg. <1, <["player", {health: 100, isRunning: true}], ["motion", {position: {34.2, 57.9}}]>>

This information is then used to create entities and their components. Some components in an entity are unique.
For example, the player JEFF has a unique component called "player". This info is used to create the player entity.

*/

// Load the game state from a JSON file
void GameSaveManager::load_game() {
	std::ifstream file(gameSaveFilePath);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		registry.clear_all_components();

		// group all components that belong to the same entity
		groupComponentsForEntities(j);
		deserialize_containers(j);
	}
	else {
		std::cout << "Unable to open file to load game" << std::endl;
	}
}

// Group all components that belong to the same entity
void GameSaveManager::groupComponentsForEntities(const json& j) {
	// group all components that belong to the same entity
	for (auto& item : j.items()) {
		// item.key is container name and item.value is an object containing components and entities stored in arrays
		auto& containerName = item.key();
		auto& container = item.value();

		// skip gameTimer and gameScore
		if (containerName == GAMETIMER || containerName == GAMESCORE || containerName == TRAPCOUNTER ||
			containerName == NEXTSPAWNS || containerName == MAXENTITIES || containerName == SPAWNDELAYS) {
			continue;
		}

		for (size_t i = 0; i < container["entities"].size(); i++) {
			int entityID = container["entities"][i];
			if (entityComponentGroups.find(entityID) == entityComponentGroups.end()) {
				entityComponentGroups[entityID] = std::map<std::string, json>();
			}
			json component = container["components"][i];
			entityComponentGroups[entityID].insert({ containerName, component });
		}
	}

	// print the map
	for (const auto& entity : entityComponentGroups) {
		std::cout << "Entity ID: " << entity.first << std::endl;

		for (const auto& component : entity.second) {
			std::cout << "  Component Name: " << component.first << std::endl;
			std::cout << "  Data: " << component.second.dump(4) << std::endl; 
		}

		std::cout << std::endl;
	}
}

// Start of deserialization
void GameSaveManager::deserialize_containers(const json& j) {
	// Deserialize to make entities using map
	for (auto& item : entityComponentGroups) {
		int entityID = item.first;
		auto& componentsMap = item.second;

		// componentMap is a map with component names as key and json object as value
		std::vector<std::string> componentNames;
		for (auto& component : componentsMap) {
			componentNames.push_back(component.first);
		}

		// create entity
		createEntity(componentNames, componentsMap);
	}

	// set up background components
	createMapTiles();
	createCliffs(window);
	// TODO - creating trees like this
	// createTrees(renderer);

	deserialize_game_timer(j);
	deserialize_game_score(j);
	deserialize_spawn_delays(j);
	deserialize_max_entities(j);
	deserialize_next_spawns(j);
  deserialize_traps_counter(j);
}

void GameSaveManager::createEntity(std::vector<std::string> componentNames, std::map<std::string, nlohmann::json> componentsMap) {
	if (std::find(componentNames.begin(), componentNames.end(), PLAYERS) != componentNames.end()) {
		createPlayerDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), BOARS) != componentNames.end()) {
		createBoarDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), BARBARIANS) != componentNames.end()) {
		createBarbarianDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), ARCHERS) != componentNames.end()) {
		createArcherDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), BIRDS) != componentNames.end()) {
		createBirdDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), WIZARDS) != componentNames.end()) {
		createWizardDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), TROLLS) != componentNames.end()) {
		createTrollDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), COLLECTIBLETRAPS) != componentNames.end()) {
		createCollectibleTrapDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), TRAPS) != componentNames.end()) {
		createTrapDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), HEARTS) != componentNames.end()) {
		createHeartDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), TARGETAREAS) != componentNames.end()) {
		createTargetAreaDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), DAMAGINGS) != componentNames.end()) {
		createDamagingsDeserialization(componentsMap);
	}
	// TODO - not using trees from save
	else if (std::find(componentNames.begin(), componentNames.end(), MESHPTRS) != componentNames.end()) {
		createTreeDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), OBSTACLES) != componentNames.end() &&
		std::find(componentNames.begin(), componentNames.end(), MESHPTRS) == componentNames.end()) {
		createObstacleDeserialization(componentsMap);
	}
}

void GameSaveManager::createObstacleDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	vec2 position = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	vec2 scale = { (float)componentsMap[MOTIONS]["scale"][0], (float)componentsMap[MOTIONS]["scale"][1] };
	TEXTURE_ASSET_ID textureAssetID = (TEXTURE_ASSET_ID)componentsMap[RENDERREQUESTS]["used_texture"];
	createObstacle(position, scale, textureAssetID);
}

void GameSaveManager::createPlayerDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 position = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity jeff = createJeff(position);

	// essential values to update
	Player& player = registry.players.get(jeff);
	player.health = componentsMap[PLAYERS]["health"];

	Motion& motion = registry.motions.get(jeff);
	motion.facing = { (float)componentsMap[MOTIONS]["facing"][0], (float)componentsMap[MOTIONS]["facing"][1] };

	Stamina& stamina = registry.staminas.get(jeff);
	stamina.stamina = componentsMap[STAMINAS]["stamina"];

	Trappable& trappable = registry.trappables.get(jeff);
	trappable.isTrapped = componentsMap[TRAPPABLES]["isTrapped"];

	handleKnockable(jeff, componentsMap);

	createPlayerUIHealthBar(camera->getSize());
	createPlayerStaminaBar(jeff, camera->getSize());
}

void GameSaveManager::createBoarDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 boarPosition = { (float) componentsMap[MOTIONS]["position"][0], (float) componentsMap[MOTIONS]["position"][1] };
	Entity boar = createBoar(boarPosition);

	handleBoar(boar, componentsMap);
	handleDasher(boar, componentsMap);

	if (componentsMap.find(DEATHTIMERS) != componentsMap.end()) {
		handleDeathTimer(boar, componentsMap);
	}
	else {
		handleEnemy(boar, componentsMap);
	}

	handleMotion(boar, componentsMap);
	handleTrappable(boar, componentsMap);
	handleKnockable(boar, componentsMap);
}

void GameSaveManager::createBarbarianDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 barbarianPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity barbarian = createBarbarian(barbarianPosition);

	// essential values to update
	if (componentsMap.find(DEATHTIMERS) != componentsMap.end()) {
		handleDeathTimer(barbarian, componentsMap);
	}
	else {
		handleEnemy(barbarian, componentsMap);
	}

	handleMotion(barbarian, componentsMap);
	handleTrappable(barbarian, componentsMap);
	handleKnockable(barbarian, componentsMap);
}

void GameSaveManager::createArcherDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 archerPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity archer = createArcher(archerPosition);

	// essential values to update
	handleArcher(archer, componentsMap);

	if (componentsMap.find(DEATHTIMERS) != componentsMap.end()) {
		handleDeathTimer(archer, componentsMap);
	}
	else {
		handleEnemy(archer, componentsMap);
	}

	handleMotion(archer, componentsMap);
	handleTrappable(archer, componentsMap);
	handleKnockable(archer, componentsMap);
}

void GameSaveManager::createBirdDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	
	// motion data
	vec2 position = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity bird = createBird(position);

	handleBird(bird, componentsMap);

	if (componentsMap.find(DEATHTIMERS) != componentsMap.end()) {
		handleDeathTimer(bird, componentsMap);
	}
	else {
		handleEnemy(bird, componentsMap);
	}

	handleMotion(bird, componentsMap);
	handleTrappable(bird, componentsMap);
	handleKnockable(bird, componentsMap);
}

void GameSaveManager::createWizardDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 wizardPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity wizard = createWizard(wizardPosition);

	handleWizard(wizard, componentsMap);

	if (componentsMap.find(DEATHTIMERS) != componentsMap.end()) {
		handleDeathTimer(wizard, componentsMap);
	}
	else {
		handleEnemy(wizard, componentsMap);
	}

	handleMotion(wizard, componentsMap);
	handleTrappable(wizard, componentsMap);
	handleKnockable(wizard, componentsMap);
}

void GameSaveManager::createTrollDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 trollPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity troll = createTroll(trollPosition);

	handleTroll(troll, componentsMap);

	if (componentsMap.find(DEATHTIMERS) != componentsMap.end()) {
		handleDeathTimer(troll, componentsMap);
	}
	else {
		handleEnemy(troll, componentsMap);
	}

	handleMotion(troll, componentsMap);
	handleTrappable(troll, componentsMap);
	handleKnocker(troll, componentsMap);
}

void GameSaveManager::createHeartDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 heartPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity heart = createHeart(heartPosition);

	handleMotion(heart, componentsMap);
}

void GameSaveManager::createCollectibleTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 collectibleTrapPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity collectibleTrap = createCollectibleTrap(collectibleTrapPosition);
	handleMotion(collectibleTrap, componentsMap);
}

void GameSaveManager::createTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 trapPosition = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	Entity damageTrap = createDamageTrap(trapPosition);

	Trap& damageTrapComponent = registry.traps.get(damageTrap);
	damageTrapComponent.position = { (float)componentsMap[TRAPS]["position"][0], (float)componentsMap[TRAPS]["position"][1] };

	handleMotion(damageTrap, componentsMap);
}

void GameSaveManager::createTreeDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 position = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1] };
	createTree(renderer, position);
}

void GameSaveManager::createTargetAreaDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	vec3 position = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1], 0 };
	Entity targetArea = createTargetArea(position);

	handleMotion(targetArea, componentsMap);
	handleCooldown(targetArea, componentsMap);
}

void GameSaveManager::createDamagingsDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	std::string type = componentsMap[DAMAGINGS]["type"];
	float damage = (float) componentsMap[DAMAGINGS]["damage"];

	vec3 position = { (float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1], (float)componentsMap[MOTIONS]["position"][2] };
	vec3 velocity = { (float)componentsMap[MOTIONS]["velocity"][0], (float)componentsMap[MOTIONS]["velocity"][1], (float)componentsMap[MOTIONS]["velocity"][2] };

	if (type == "arrow") {
		createArrow(position, velocity, damage);
	}
	else if (type == "fireball") {
		float angle = (float) componentsMap[MOTIONS]["angle"];
		vec2 direction = vec2(cos(angle), sin(angle));
		createFireball(position, direction);
	}
	else if (type == "lightning") {
		createLightning(position);
	}
}

void GameSaveManager::deserialize_game_timer(const json& j) {
	registry.gameTimer.hours = j[GAMETIMER]["hour"].get<int>();
	registry.gameTimer.minutes = j[GAMETIMER]["minute"].get<int>();
	registry.gameTimer.seconds = j[GAMETIMER]["second"].get<int>();
	registry.gameTimer.elapsed = j[GAMETIMER]["elapsed"].get<float>();
	registry.gameTimer.ms = j[GAMETIMER]["ms"].get<int>();

	printf("Game Timer: %d:%d:%d:%d\n", registry.gameTimer.hours, registry.gameTimer.minutes, registry.gameTimer.seconds, registry.gameTimer.ms);
}

void GameSaveManager::deserialize_game_score(const json& j) {
	registry.gameScore.highScoreHours = j[GAMESCORE]["highScoreHours"].get<int>();
	registry.gameScore.highScoreMinutes = j[GAMESCORE]["highScoreMinutes"].get<int>();
	registry.gameScore.highScoreSeconds = j[GAMESCORE]["highScoreSeconds"].get<int>();
}

void GameSaveManager::deserialize_spawn_delays(const json& j) {
	auto& spawnDelaysJ = j.at(SPAWNDELAYS);
	for (const auto& delay : spawnDelaysJ.items()) {
		spawnDelays[delay.key()] = delay.value().get<float>();
	}
}

void GameSaveManager::deserialize_max_entities(const json& j) {
	auto& maxEntitiesJ = j[MAXENTITIES];
	for (const auto& max : maxEntitiesJ.items()) {
		maxEntities[max.key()] = max.value().get<int>();
	}
}

void GameSaveManager::deserialize_next_spawns(const json& j) {
	auto& nextSpawnsJ = j[NEXTSPAWNS];
	for (const auto& next : nextSpawnsJ.items()) {
		nextSpawns[next.key()] = next.value().get<float>();
    	}
}

void GameSaveManager::deserialize_traps_counter(const json& j) {
	for (const auto& trap : j[TRAPCOUNTER].items()) {
		std::string trapName = trap.key();
		int count = trap.value()["count"];
		Entity textEntity;
		if (trapName == "trap") {
			textEntity = createItemCountText(camera->getSize(), TEXTURE_ASSET_ID::TRAPCOLLECTABLE);
		}
		else {
			textEntity = createItemCountText(camera->getSize(), TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE);
		}
		this->trapsCounter[trapName] = std::make_pair(count, textEntity);
	}
}


// DESERIALIZATION HELPERS (COMPONENTS)
void GameSaveManager::handleDeathTimer(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	// remove from enemies container first (because creating entity puts the enemy automatically into enemey container)
	registry.enemies.remove(entity);

	DeathTimer& deathTimer = registry.deathTimers.emplace(entity);
	AnimationController& animationController = registry.animationControllers.get(entity);
	animationController.changeState(entity, AnimationState::Dead);
	deathTimer.timer = componentsMap[DEATHTIMERS]["timer"];
	HealthBar& hpbar = registry.healthBars.get(entity);
	registry.remove_all_components_of(hpbar.meshEntity);
	registry.remove_all_components_of(hpbar.frameEntity);
	registry.healthBars.remove(entity);
}

void GameSaveManager::handleMotion(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Motion& motion = registry.motions.get(entity);
	motion.position = vec3((float)componentsMap[MOTIONS]["position"][0], (float)componentsMap[MOTIONS]["position"][1], (float)componentsMap[MOTIONS]["position"][2]);
	motion.angle = componentsMap[MOTIONS]["angle"];
	motion.velocity = { (float)componentsMap[MOTIONS]["velocity"][0], (float)componentsMap[MOTIONS]["velocity"][1], 0 };
	motion.speed = componentsMap[MOTIONS]["speed"];
	motion.scale = { (float)componentsMap[MOTIONS]["scale"][0], (float)componentsMap[MOTIONS]["scale"][1] };
	motion.facing = { (float)componentsMap[MOTIONS]["facing"][0], (float)componentsMap[MOTIONS]["facing"][1] };
	motion.hitbox = { (float)componentsMap[MOTIONS]["hitbox"][0], (float)componentsMap[MOTIONS]["hitbox"][1], (float)componentsMap[MOTIONS]["hitbox"][2] };
	motion.gravity = componentsMap[MOTIONS]["gravity"];
	motion.solid = componentsMap[MOTIONS]["solid"];
}

void GameSaveManager::handleTrappable(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Trappable& trappable = registry.trappables.get(entity);
	trappable.isTrapped = componentsMap[TRAPPABLES]["isTrapped"];
	trappable.originalSpeed = componentsMap[TRAPPABLES]["originalSpeed"];
}

void GameSaveManager::handleEnemy(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Enemy& enemy = registry.enemies.get(entity);
	enemy.health = componentsMap[ENEMIES]["health"];
	enemy.damage = componentsMap[ENEMIES]["damage"];
	enemy.type = componentsMap[ENEMIES]["type"];
	enemy.cooldown = componentsMap[ENEMIES]["cooldown"];
	enemy.pathfindTime = componentsMap[ENEMIES]["pathfindTime"];
}

void GameSaveManager::handleDasher(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Dash& dash = registry.dashers.get(entity);
	dash.isDashing = componentsMap[DASHERS]["isDashing"];
	dash.dashStartPosition = { (float)componentsMap[DASHERS]["dashStartPosition"][0], (float)componentsMap[DASHERS]["dashStartPosition"][1] };
	dash.dashTargetPosition = { (float)componentsMap[DASHERS]["dashTargetPosition"][0], (float)componentsMap[DASHERS]["dashTargetPosition"][1] };
	dash.dashTimer = (float)componentsMap[DASHERS]["dashTimer"];
	dash.dashDuration = (float)componentsMap[DASHERS]["dashDuration"];
}

void GameSaveManager::handleKnocker(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Knocker& knocker = registry.knockers.get(entity);
	knocker.strength = componentsMap[KNOCKERS]["strength"];
}

void GameSaveManager::handleKnockable(Entity& entity, std::map<std::string, nlohmann::json> componentsMap)
{
	Knockable& knockable = registry.knockables.get(entity);
	knockable.knocked = componentsMap[KNOCKABLES]["knocked"];
}

void GameSaveManager::handleCooldown(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Cooldown& cooldown = registry.cooldowns.get(entity);
	cooldown.remaining = componentsMap[COOLDOWNS]["remaining"];
}

void GameSaveManager::handleBoar(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Boar& boar = registry.boars.get(entity);
	boar.cooldownTimer = (float)componentsMap[BOARS]["cooldownTimer"];
	boar.prepareTimer = (float)componentsMap[BOARS]["prepareTimer"];
	boar.chargeTimer = (float)componentsMap[BOARS]["chargeTimer"];
	boar.preparing = componentsMap[BOARS]["preparing"];
	boar.charging = componentsMap[BOARS]["charging"];
	boar.chargeDirection = { (float)componentsMap[BOARS]["chargeDirection"][0], (float)componentsMap[BOARS]["chargeDirection"][1] };
}

void GameSaveManager::handleBarbarian(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Enemy& enemy = registry.enemies.get(entity);
	enemy.health = componentsMap[ENEMIES]["health"];
	enemy.damage = componentsMap[ENEMIES]["damage"];
	enemy.type = componentsMap[ENEMIES]["type"];
	enemy.cooldown = componentsMap[ENEMIES]["cooldown"];
	enemy.pathfindTime = componentsMap[ENEMIES]["pathfindTime"];
}

void GameSaveManager::handleArcher(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Archer& archer = registry.archers.get(entity);
	archer.drawArrowTime = componentsMap[ARCHERS]["drawArrowTime"];
	archer.aiming = componentsMap[ARCHERS]["aiming"];
}

void GameSaveManager::handleBird(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Bird& bird = registry.birds.get(entity);
	bird.swarmSpeed = componentsMap[BIRDS]["swarmSpeed"];
	bird.swoopSpeed = componentsMap[BIRDS]["swoopSpeed"];
	bird.isSwooping = componentsMap[BIRDS]["isSwooping"];
	bird.swoopTimer = componentsMap[BIRDS]["swoopTimer"];
	bird.swoopDirection = { (float)componentsMap[BIRDS]["swoopDirection"][0], (float)componentsMap[BIRDS]["swoopDirection"][1] };
	bird.originalZ = componentsMap[BIRDS]["originalZ"];
	bird.swoopCooldown = componentsMap[BIRDS]["swoopCooldown"];
}

void GameSaveManager::handleWizard(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Wizard& wizard = registry.wizards.get(entity);
	wizard.state = componentsMap[WIZARDS]["state"];
	wizard.shoot_cooldown = componentsMap[WIZARDS]["shoot_cooldown"];
	wizard.prepareLightningTime = componentsMap[WIZARDS]["prepareLightningTime"];
	wizard.locked_target = { (float)componentsMap[WIZARDS]["locked_target"][0], (float)componentsMap[WIZARDS]["locked_target"][1], (float)componentsMap[WIZARDS]["locked_target"][2] };
}

void GameSaveManager::handleTroll(Entity& entity, std::map<std::string, nlohmann::json> componentsMap) {
	Troll& troll = registry.trolls.get(entity);
	troll.desiredAngle = componentsMap[TROLLS]["desiredAngle"];
}

void GameSaveManager::loadTrapsCounter(std::unordered_map<std::string, std::pair<int, Entity>>& trapCounterWorld) {
	// assign entries from local traps counter to world traps counter
	for (auto& entry : this->trapsCounter) {
		trapCounterWorld[entry.first] = entry.second;
		printf("Trap: %s, Count: %d\n", entry.first.c_str(), entry.second.first);	
	}
}

std::unordered_map<std::string, float> GameSaveManager::getSpawnDelays() {
	return this->spawnDelays;
}

std::unordered_map<std::string, int> GameSaveManager::getMaxEntities() {
	return this->maxEntities;
}

std::unordered_map<std::string, float> GameSaveManager::getNextSpawns() {
	return this->nextSpawns;
}



