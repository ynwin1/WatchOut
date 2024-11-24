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

GameSaveManager::GameSaveManager(RenderSystem* renderer, GLFWwindow* window, Camera* camera) {
	this->renderer = renderer;
	this->window = window;
	this->camera = camera;
}

// Serialize the game state to a JSON file
void GameSaveManager::save_game() {
	json j;

	serialize_containers(j);

	std::ofstream file(gameSaveFilePath);
	if (file.is_open()) {
		file << j.dump(4);
		file.close();
	}
	else {
		std::cout << "Unable to open file to save game" << std::endl;
	}
}

void GameSaveManager::serialize_containers(json& j) {
	j["gameTimer"] = serialize_game_timer(registry.gameTimer);
	j["gameScore"] = serialize_game_score(registry.gameScore);

	// Serialize all component containers
	j["players"] = serialize_container<Player>(registry.players);
	j["dashers"] = serialize_container<Dash>(registry.dashers);
	j["enemies"] = serialize_container<Enemy>(registry.enemies);
	j["motions"] = serialize_container<Motion>(registry.motions);
	j["collisions"] = serialize_container<Collision>(registry.collisions);
	j["cooldowns"] = serialize_container<Cooldown>(registry.cooldowns);
	j["collectibles"] = serialize_container<Collectible>(registry.collectibles);
	j["traps"] = serialize_container<Trap>(registry.traps);
	j["damageds"] = serialize_container<Damaged>(registry.damageds);
	j["damagings"] = serialize_container<Damaging>(registry.damagings);
	j["deathTimers"] = serialize_container<DeathTimer>(registry.deathTimers);
	j["knockables"] = serialize_container<Knockable>(registry.knockables);
	j["knockers"] = serialize_container<Knocker>(registry.knockers);
	j["trappables"] = serialize_container<Trappable>(registry.trappables);
	j["healthBars"] = serialize_container<HealthBar>(registry.healthBars);
	j["animationControllers"] = serialize_container<AnimationController>(registry.animationControllers);
	j["staminaBars"] = serialize_container<StaminaBar>(registry.staminaBars);
	j["staminas"] = serialize_container<Stamina>(registry.staminas);
	j["texts"] = serialize_container<Text>(registry.texts);
	j["jumpers"] = serialize_container<Jumper>(registry.jumpers);
	j["mapTiles"] = serialize_container<MapTile>(registry.mapTiles);
	j["obstacles"] = serialize_container<Obstacle>(registry.obstacles);
	j["projectiles"] = serialize_container<Projectile>(registry.projectiles);
	j["meshPtrs"] = serialize_mesh_container(registry.meshPtrs); // SPECIAL CASE
	j["targetAreas"] = serialize_container<TargetArea>(registry.targetAreas);
	j["collected"] = serialize_container<Collected>(registry.collected);
	// j["pauseMenuComponents"] = serialize_container<PauseMenuComponent>(registry.pauseMenuComponents);
	// j["helpMenuComponents"] = serialize_container<HelpMenuComponent>(registry.helpMenuComponents);
	j["renderRequests"] = serialize_container<RenderRequest>(registry.renderRequests);
	j["backgrounds"] = serialize_container<Background>(registry.backgrounds);
	j["midgrounds"] = serialize_container<Midground>(registry.midgrounds);
	j["foregrounds"] = serialize_container<Foreground>(registry.foregrounds);
	j["colours"] = serialize_container<vec4>(registry.colours);
	j["boars"] = serialize_container<Boar>(registry.boars);
	j["babarians"] = serialize_container<Barbarian>(registry.barbarians);
	j["archers"] = serialize_container<Archer>(registry.archers);
	j["birds"] = serialize_container<Bird>(registry.birds);
	j["wizards"] = serialize_container<Wizard>(registry.wizards);
	j["trolls"] = serialize_container<Troll>(registry.trolls);
	j["hearts"] = serialize_container<Heart>(registry.hearts);
	j["collectibleTraps"] = serialize_container<CollectibleTrap>(registry.collectibleTraps);
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
	j["position"] = { motion.position.x, motion.position.y };
	j["angle"] = motion.angle;
	j["velocity"] = { motion.velocity.x, motion.velocity.y };
	j["speed"] = motion.speed;
	j["scale"] = { motion.scale.x, motion.scale.y };
	j["facing"] = { motion.facing.x, motion.facing.y };
	j["hitbox"] = { motion.hitbox.x, motion.hitbox.y, motion.hitbox.z };
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
	j["position"] = { mapTile.position.x, mapTile.position.y };
	j["scale"] = { mapTile.scale.x, mapTile.scale.y };
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


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% //

// DESERIALIZATION

// Load the game state from a JSON file
void GameSaveManager::load_game() {
	std::ifstream file(gameSaveFilePath);
	if (file.is_open()) {
		json j;
		file >> j;
		// printf("Loaded game state is : %s\n", j.dump(4).c_str());
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

void GameSaveManager::groupComponentsForEntities(const json& j) {
	// group all components that belong to the same entity
	for (auto& item : j.items()) {
		// item.key is container name and item.value is an object containing components and entities stored in arrays
		auto& containerName = item.key();
		auto& container = item.value();

		// skip gameTimer and gameScore
		if (containerName == "gameTimer" || containerName == "gameScore") {
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
	//for (const auto& entity : entityComponentGroups) {
	//	std::cout << "Entity ID: " << entity.first << std::endl;

	//	for (const auto& component : entity.second) {
	//		std::cout << "  Component Name: " << component.first << std::endl;
	//		std::cout << "  Data: " << component.second.dump(4) << std::endl;  // Using dump(4) for pretty-printing with an indent of 4 spaces
	//	}

	//	std::cout << std::endl;  // Add a newline for better separation between entities
	//}
}

void GameSaveManager::deserialize_containers(const json& j) {

	// Deserialize to make entities using map

	//auto& item = *(entityComponentGroups.begin());
	//auto& entityID = item.first;
	//auto& components = item.second;
	//for (const auto& component : components) {
	//	std::cout << component.first << std::endl;
	//}

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

	deserialize_game_timer(j);
	deserialize_game_score(j);
}

void GameSaveManager::createEntity(std::vector<std::string> componentNames, std::map<std::string, nlohmann::json> componentsMap) {
	// map related components
	if (std::find(componentNames.begin(), componentNames.end(), "players") != componentNames.end()) {
		createPlayerDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "boars") != componentNames.end()) {
		createBoarDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "barbarians") != componentNames.end()) {
		createBarbarianDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "archers") != componentNames.end()) {
		createArcherDeserialization(componentsMap);
	}
	//else if (std::find(componentNames.begin(), componentNames.end(), "birds") != componentNames.end()) {
	//	createBirdFlockDeserialization(componentsMap);
	//}
	else if (std::find(componentNames.begin(), componentNames.end(), "wizards") != componentNames.end()) {
		createWizardDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "trolls") != componentNames.end()) {
		createTrollDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "collectibleTraps") != componentNames.end()) {
		createCollectibleTrapDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "traps") != componentNames.end()) {
		createTrapDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "hearts") != componentNames.end()) {
		createHeartDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "targetAreas") != componentNames.end()) {
		createTargetAreaDeserialization(componentsMap);
	}
	else if (std::find(componentNames.begin(), componentNames.end(), "damagings") != componentNames.end()) {
		createDamagingsDeserialization(componentsMap);
	}
	/*else if (std::find(componentNames.begin(), componentNames.end(), "meshPtrs") != componentNames.end()) {
		createTreeDeserialization(componentsMap);
	}*/
	/*else if (std::find(componentNames.begin(), componentNames.end(), "obstacles") != componentNames.end()) {
		createObstacleDeserialization(componentsMap);
	}*/
}

void GameSaveManager::createObstacleDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	vec2 position = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	vec2 scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	TEXTURE_ASSET_ID textureAssetID = (TEXTURE_ASSET_ID)componentsMap["renderRequests"]["used_texture"];
	createObstacle(position, scale, textureAssetID);
}

void GameSaveManager::createPlayerDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 position = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity jeff = createJeff(position);

	// essential values to update
	Player& player = registry.players.get(jeff);
	player.health = componentsMap["players"]["health"];

	Stamina& stamina = registry.staminas.get(jeff);
	stamina.stamina = componentsMap["staminas"]["stamina"];

	Trappable& trappable = registry.trappables.get(jeff);
	trappable.isTrapped = componentsMap["trappables"]["isTrapped"];

	createPlayerHealthBar(jeff, camera->getSize());
	createPlayerStaminaBar(jeff, camera->getSize());
}

void GameSaveManager::createBoarDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 boarPosition = { (float) componentsMap["motions"]["position"][0], (float) componentsMap["motions"]["position"][1] };
	Entity boar = createBoar(boarPosition);

	Boar& boarComponent = registry.boars.get(boar);
	boarComponent.cooldownTimer = (float) componentsMap["boars"]["cooldownTimer"];
	boarComponent.prepareTimer = (float) componentsMap["boars"]["prepareTimer"];
	boarComponent.chargeTimer = (float) componentsMap["boars"]["chargeTimer"];
	boarComponent.preparing = componentsMap["boars"]["preparing"];
	boarComponent.charging = componentsMap["boars"]["charging"];
	boarComponent.chargeDirection = { (float)componentsMap["boars"]["chargeDirection"][0], (float)componentsMap["boars"]["chargeDirection"][1] };

	Dash& dash = registry.dashers.get(boar);
	dash.isDashing = componentsMap["dashers"]["isDashing"];
	dash.dashStartPosition = { (float)componentsMap["dashers"]["dashStartPosition"][0], (float)componentsMap["dashers"]["dashStartPosition"][1] };
	dash.dashTargetPosition = { (float)componentsMap["dashers"]["dashTargetPosition"][0], (float)componentsMap["dashers"]["dashTargetPosition"][1] };
	dash.dashTimer = (float)componentsMap["dashers"]["dashTimer"];
	dash.dashDuration = (float)componentsMap["dashers"]["dashDuration"];

	Enemy& enemy = registry.enemies.get(boar);
	enemy.health = componentsMap["enemies"]["health"];
	enemy.damage = componentsMap["enemies"]["damage"];
	enemy.type = componentsMap["enemies"]["type"];
	enemy.cooldown = componentsMap["enemies"]["cooldown"];
	enemy.pathfindTime = componentsMap["enemies"]["pathfindTime"];

	Motion& motion = registry.motions.get(boar);
	motion.position = vec3(boarPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.velocity = { (float)componentsMap["motions"]["velocity"][0], (float)componentsMap["motions"]["velocity"][1], 0 };
	motion.speed = componentsMap["motions"]["speed"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.facing = { (float)componentsMap["motions"]["facing"][0], (float)componentsMap["motions"]["facing"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
	motion.solid = componentsMap["motions"]["solid"];
	
	Trappable& trappable = registry.trappables.get(boar);
	trappable.isTrapped = componentsMap["trappables"]["isTrapped"];
	trappable.originalSpeed = componentsMap["trappables"]["originalSpeed"];
}

void GameSaveManager::createBarbarianDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 barbarianPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity barbarian = createBarbarian(barbarianPosition);

	// essential values to update
	Enemy& enemy = registry.enemies.get(barbarian);
	enemy.health = componentsMap["enemies"]["health"];
	enemy.damage = componentsMap["enemies"]["damage"];
	enemy.type = componentsMap["enemies"]["type"];
	enemy.cooldown = componentsMap["enemies"]["cooldown"];
	enemy.pathfindTime = componentsMap["enemies"]["pathfindTime"];

	Motion& motion = registry.motions.get(barbarian);
	motion.position = vec3(barbarianPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.velocity = { (float)componentsMap["motions"]["velocity"][0], (float)componentsMap["motions"]["velocity"][1], 0 };
	motion.speed = componentsMap["motions"]["speed"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.facing = { (float)componentsMap["motions"]["facing"][0], (float)componentsMap["motions"]["facing"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
	motion.solid = componentsMap["motions"]["solid"];

	Trappable& trappable = registry.trappables.get(barbarian);
	trappable.isTrapped = componentsMap["trappables"]["isTrapped"];
	trappable.originalSpeed = componentsMap["trappables"]["originalSpeed"];
}

void GameSaveManager::createArcherDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 archerPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity archer = createArcher(archerPosition);

	// essential values to update
	Archer& archerComponent = registry.archers.get(archer);
	archerComponent.drawArrowTime = componentsMap["archers"]["drawArrowTime"];
	archerComponent.aiming = componentsMap["archers"]["aiming"];

	Enemy& enemy = registry.enemies.get(archer);
	enemy.health = componentsMap["enemies"]["health"];
	enemy.health = componentsMap["enemies"]["health"];
	enemy.damage = componentsMap["enemies"]["damage"];
	enemy.type = componentsMap["enemies"]["type"];
	enemy.cooldown = componentsMap["enemies"]["cooldown"];
	enemy.pathfindTime = componentsMap["enemies"]["pathfindTime"];

	Motion& motion = registry.motions.get(archer);
	motion.position = vec3(archerPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.velocity = { (float)componentsMap["motions"]["velocity"][0], (float)componentsMap["motions"]["velocity"][1], 0 };
	motion.speed = componentsMap["motions"]["speed"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.facing = { (float)componentsMap["motions"]["facing"][0], (float)componentsMap["motions"]["facing"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
	motion.solid = componentsMap["motions"]["solid"];

	Trappable& trappable = registry.trappables.get(archer);
	trappable.isTrapped = componentsMap["trappables"]["isTrapped"];
	trappable.originalSpeed = componentsMap["trappables"]["originalSpeed"];

}

// TODO
void GameSaveManager::createBirdFlockDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	
	// motion data
	vec2 position = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	createBirdFlock(position);
}

void GameSaveManager::createWizardDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 wizardPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity wizard = createWizard(wizardPosition);

	Wizard& wizardComponent = registry.wizards.get(wizard);
	wizardComponent.state = (WizardState) componentsMap["wizards"]["state"];
	wizardComponent.shoot_cooldown = componentsMap["wizards"]["shoot_cooldown"];
	wizardComponent.prepareLightningTime = componentsMap["wizards"]["prepareLightningTime"];
	wizardComponent.locked_target = { (float)componentsMap["wizards"]["locked_target"][0], (float)componentsMap["wizards"]["locked_target"][1], (float)componentsMap["wizards"]["locked_target"][2] };

	Enemy& enemy = registry.enemies.get(wizard);
	enemy.health = componentsMap["enemies"]["health"];
	enemy.damage = componentsMap["enemies"]["damage"];
	enemy.type = componentsMap["enemies"]["type"];
	enemy.cooldown = componentsMap["enemies"]["cooldown"];
	enemy.pathfindTime = componentsMap["enemies"]["pathfindTime"];

	Motion& motion = registry.motions.get(wizard);
	motion.position = vec3(wizardPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.velocity = { (float)componentsMap["motions"]["velocity"][0], (float)componentsMap["motions"]["velocity"][1], 0 };
	motion.speed = componentsMap["motions"]["speed"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.facing = { (float)componentsMap["motions"]["facing"][0], (float)componentsMap["motions"]["facing"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
	motion.solid = componentsMap["motions"]["solid"];

	Trappable& trappable = registry.trappables.get(wizard);
	trappable.isTrapped = componentsMap["trappables"]["isTrapped"];
	trappable.originalSpeed = componentsMap["trappables"]["originalSpeed"];
}

void GameSaveManager::createTrollDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 trollPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity troll = createTroll(trollPosition);

	Troll& trollComponent = registry.trolls.get(troll);
	trollComponent.desiredAngle = componentsMap["trolls"]["desiredAngle"];

	Enemy& enemy = registry.enemies.get(troll);
	enemy.health = componentsMap["enemies"]["health"];
	enemy.damage = componentsMap["enemies"]["damage"];
	enemy.type = componentsMap["enemies"]["type"];
	enemy.cooldown = componentsMap["enemies"]["cooldown"];
	enemy.pathfindTime = componentsMap["enemies"]["pathfindTime"];

	Motion& motion = registry.motions.get(troll);
	motion.position = vec3(trollPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.velocity = { (float)componentsMap["motions"]["velocity"][0], (float)componentsMap["motions"]["velocity"][1], 0 };
	motion.speed = componentsMap["motions"]["speed"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.facing = { (float)componentsMap["motions"]["facing"][0], (float)componentsMap["motions"]["facing"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
	motion.solid = componentsMap["motions"]["solid"];

	Trappable& trappable = registry.trappables.get(troll);
	trappable.isTrapped = componentsMap["trappables"]["isTrapped"];
	trappable.originalSpeed = componentsMap["trappables"]["originalSpeed"];
}

void GameSaveManager::createHeartDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 heartPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity heart = createHeart(heartPosition);

	Motion& motion = registry.motions.get(heart);
	motion.position = vec3(heartPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
}

void GameSaveManager::createCollectibleTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 collectibleTrapPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity collectibleTrap = createCollectibleTrap(collectibleTrapPosition);

	Motion& motion = registry.motions.get(collectibleTrap);
	motion.position = vec3(collectibleTrapPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
}

void GameSaveManager::createTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 trapPosition = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity damageTrap = createDamageTrap(trapPosition);

	Trap& damageTrapComponent = registry.traps.get(damageTrap);
	damageTrapComponent.position = { (float)componentsMap["traps"]["position"][0], (float)componentsMap["traps"]["position"][1] };

	Motion& motion = registry.motions.get(damageTrap);
	motion.position = vec3(trapPosition, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };
}

void GameSaveManager::createTreeDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	// motion data
	vec2 position = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1] };
	Entity tree = createTree(renderer, position);

	// Motion
	Motion& motion = registry.motions.get(tree);
	motion.position = vec3(position, motion.position.z);
	motion.angle = componentsMap["motions"]["angle"];
	motion.scale = { (float)componentsMap["motions"]["scale"][0], (float)componentsMap["motions"]["scale"][1] };
	motion.hitbox = { (float)componentsMap["motions"]["hitbox"][0], (float)componentsMap["motions"]["hitbox"][1], (float)componentsMap["motions"]["hitbox"][2] };

}

void GameSaveManager::createTargetAreaDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	vec3 position = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1], 0 };
	Entity targetArea = createTargetArea(position);

	Cooldown& cooldown = registry.cooldowns.get(targetArea);
	cooldown.remaining = componentsMap["cooldowns"]["remaining"];
}

void GameSaveManager::createDamagingsDeserialization(std::map<std::string, nlohmann::json> componentsMap) {
	std::string type = componentsMap["damagings"]["type"];
	float damage = (float) componentsMap["damagings"]["damage"];

	vec3 position = { (float)componentsMap["motions"]["position"][0], (float)componentsMap["motions"]["position"][1], (float)componentsMap["motions"]["position"][2] };
	vec3 velocity = { (float)componentsMap["motions"]["velocity"][0], (float)componentsMap["motions"]["velocity"][1], (float)componentsMap["motions"]["velocity"][2] };

	if (type == "arrow") {
		createArrow(position, velocity, damage);
	}
	else if (type == "fireball") {
		float angle = (float) componentsMap["motions"]["angle"];
		vec2 direction = vec2(cos(angle), sin(angle));
		createFireball(position, direction);
	}
	else if (type == "lightning") {
		createLightning(position);
	}
}

void GameSaveManager::deserialize_game_timer(const json& j) {
	registry.gameTimer.hours = j["gameTimer"]["hour"].get<int>();
	registry.gameTimer.minutes = j["gameTimer"]["minute"].get<int>();
	registry.gameTimer.seconds = j["gameTimer"]["second"].get<int>();
	registry.gameTimer.ms = j["gameTimer"]["ms"].get<int>();

	printf("Game Timer: %d:%d:%d:%d\n", registry.gameTimer.hours, registry.gameTimer.minutes, registry.gameTimer.seconds, registry.gameTimer.ms);
}

void GameSaveManager::deserialize_game_score(const json& j) {
	registry.gameScore.highScoreHours = j["gameScore"]["highScoreHours"].get<int>();
	registry.gameScore.highScoreMinutes = j["gameScore"]["highScoreMinutes"].get<int>();
	registry.gameScore.highScoreSeconds = j["gameScore"]["highScoreSeconds"].get<int>();
}

