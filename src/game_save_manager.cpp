#include "game_save_manager.hpp"
#include "tiny_ecs_registry.hpp"
#include "json.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Serialize the game state to a JSON file
void GameSaveManager::save_game() {
	json j;

	serialize_containers(j);

	printf("Serialized\n");
	std::string filepath = data_path() + "/save/game_save.json";
	printf("Printed JSON is %s\n", j.dump(4).c_str());
	std::ofstream file(filepath);
	if (file.is_open()) {
		file << j.dump(4);
		file.close();
		printf("Game saved\n");
	}
	else {
		std::cout << "Unable to open file to save game" << std::endl;
	}
}

void GameSaveManager::serialize_containers(json& j) {

	j["players"] = serialize_container<Player>(registry.players);
	j["dashers"] = serialize_container<Dash>(registry.dashers);
	//j["enemies"] = serialize_container<Enemy>(registry.enemies);
	j["motions"] = serialize_container<Motion>(registry.motions);
	//j["collisions"] = serialize_container<Collision>(registry.collisions);
	//j["cooldowns"] = serialize_container<Cooldown>(registry.cooldowns);
	//j["collectibles"] = serialize_container<Collectible>(registry.collectibles);
	//j["traps"] = serialize_container<Trap>(registry.traps);
	//j["damageds"] = serialize_container<Damaged>(registry.damageds);
	//j["damagings"] = serialize_container<Damaging>(registry.damagings);
	//j["deathTimers"] = serialize_container<DeathTimer>(registry.deathTimers);
	//j["knockables"] = serialize_container<Knockable>(registry.knockables);
	//j["knockers"] = serialize_container<Knocker>(registry.knockers);
	//j["trappables"] = serialize_container<Trappable>(registry.trappables);
	//j["healthBars"] = serialize_container<HealthBar>(registry.healthBars);
	//j["animationControllers"] = serialize_container<AnimationController>(registry.animationControllers);
	//j["staminaBars"] = serialize_container<StaminaBar>(registry.staminaBars);
	j["staminas"] = serialize_container<Stamina>(registry.staminas);
	//j["texts"] = serialize_container<Text>(registry.texts);
	//j["jumpers"] = serialize_container<Jumper>(registry.jumpers);
	//j["mapTiles"] = serialize_container<MapTile>(registry.mapTiles);
	//j["obstacles"] = serialize_container<Obstacle>(registry.obstacles);
	//j["projectiles"] = serialize_container<Projectile>(registry.projectiles);
	//j["meshPtrs"] = serialize_container<Mesh*>(registry.meshPtrs);
	//j["targetAreas"] = serialize_container<TargetArea>(registry.targetAreas);
	//j["collected"] = serialize_container<Collected>(registry.collected);
	//j["pauseMenuComponents"] = serialize_container<PauseMenuComponent>(registry.pauseMenuComponents);
	//j["helpMenuComponents"] = serialize_container<HelpMenuComponent>(registry.helpMenuComponents);
	//j["renderRequests"] = serialize_container<RenderRequest>(registry.renderRequests);
	//j["backgrounds"] = serialize_container<Background>(registry.backgrounds);
	//j["midgrounds"] = serialize_container<Midground>(registry.midgrounds);
	//j["foregrounds"] = serialize_container<Foreground>(registry.foregrounds);
	//j["colours"] = serialize_container<vec4>(registry.colours);
	//j["boars"] = serialize_container<Boar>(registry.boars);
	//j["babarians"] = serialize_container<Barbarian>(registry.barbarians);
	//j["archers"] = serialize_container<Archer>(registry.archers);
	//j["birds"] = serialize_container<Bird>(registry.birds);
	//j["wizards"] = serialize_container<Wizard>(registry.wizards);
	//j["trolls"] = serialize_container<Troll>(registry.trolls);
	//j["hearts"] = serialize_container<Heart>(registry.hearts);
	//j["collectibleTraps"] = serialize_container<CollectibleTrap>(registry.collectibleTraps);
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

// Load the game state from a JSON file
void GameSaveManager::load_game() {

}

// SERIALIZING COMPONENTS
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
nlohmann::json GameSaveManager::serialize_component<Stamina>(const Stamina& stamina) {
	nlohmann::json j;
	j["stamina"] = stamina.stamina;
	j["max_stamina"] = stamina.max_stamina;
	j["stamina_loss_rate"] = stamina.stamina_loss_rate;
	j["stamina_recovery_rate"] = stamina.stamina_recovery_rate;
	j["timer"] = stamina.timer;
	return j;
}