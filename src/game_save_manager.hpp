#pragma once
#include "json.hpp"
#include "tiny_ecs.hpp"

class GameSaveManager {
public:
	using json = nlohmann::json;

	// Save the game
	void save_game();

	// Load the game
	void load_game();

private:

	// Serialization
	void serialize_containers(json& j);

	template <typename Component>
	nlohmann::json serialize_container(const ComponentContainer<Component>& container);
};