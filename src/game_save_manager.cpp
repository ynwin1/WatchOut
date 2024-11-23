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
	// j["animationControllers"] = serialize_container<AnimationController>(registry.animationControllers);
	j["staminaBars"] = serialize_container<StaminaBar>(registry.staminaBars);
	j["staminas"] = serialize_container<Stamina>(registry.staminas);
	j["texts"] = serialize_container<Text>(registry.texts);
	j["jumpers"] = serialize_container<Jumper>(registry.jumpers);
	j["mapTiles"] = serialize_container<MapTile>(registry.mapTiles);
	j["obstacles"] = serialize_container<Obstacle>(registry.obstacles);
	j["projectiles"] = serialize_container<Projectile>(registry.projectiles);
	// j["meshPtrs"] = serialize_container<Mesh*>(registry.meshPtrs);
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

// Load the game state from a JSON file
void GameSaveManager::load_game() {

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

// TODO - ANIMATION CONTROLLER
//template<>
//nlohmann::json GameSaveManager::serialize_component<AnimationController>(const AnimationController& animationController) {
//	nlohmann::json j;
//	j["currentAnimation"] = animationController.currentAnimation;
//	j["currentFrame"] = animationController.currentFrame;
//	j["frameTimer"] = animationController.frameTimer;
//	j["frameDuration"] = animationController.frameDuration;
//	j["loop"] = animationController.loop;
//	return j;
//}

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

// TODO - MESH
//template<>
//nlohmann::json GameSaveManager::serialize_component<Mesh*>(const Mesh*& meshPtr) {
//	nlohmann::json j;
//	j["originalSize"] = { meshPtr->originalSize.x, meshPtr->originalSize.y };
//	j["vertices"] = json::array();
//	return j;
//}

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

// TODO - PAUSE AND HELP MENUS

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

