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
	j["staminas"] = serialize_container<Stamina>(registry.staminas);
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
nlohmann::json GameSaveManager::serialize_component<Stamina>(const Stamina& stamina) {
	nlohmann::json j;
	j["stamina"] = stamina.stamina;
	j["max_stamina"] = stamina.max_stamina;
	j["stamina_loss_rate"] = stamina.stamina_loss_rate;
	j["stamina_recovery_rate"] = stamina.stamina_recovery_rate;
	j["timer"] = stamina.timer;
	return j;
}