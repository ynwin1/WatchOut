#pragma once
#include "json.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"

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
	json serialize_container(const ComponentContainer<Component>& container);

	template <typename Component>
	nlohmann::json serialize_component(const Component& component);
};