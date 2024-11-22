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

	std::ofstream file("game_save.json");
	if (file.is_open()) {
		file << j.dump(4);
		file.close();
	}
	else {
		std::cout << "Unable to open file to save game" << std::endl;
	}
}

void GameSaveManager::serialize_containers(json& j) {

	j["players"] = serialize_container<Player>(registry.players);
}

template <typename Component>
nlohmann::json GameSaveManager::serialize_container(const ComponentContainer<Component>& container) {
	json j_array = json::array();

	/*for (const auto& component : container.components) {
		j_array.push_back(serialize_component(component));
	}*/

	return j_array;
}

// Load the game state from a JSON file
void GameSaveManager::load_game() {

}