#pragma once
#include "json.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include <render_system.hpp>
#include "camera.hpp"

class GameSaveManager {
public:
	using json = nlohmann::json;

	GameSaveManager(RenderSystem* renderer, GLFWwindow* window, Camera* camera);

	// Save the game
	void save_game();

	// Load the game
	void load_game();

private:

	RenderSystem* renderer;
	GLFWwindow* window;
	Camera* camera;

	// Game Save file path
	std::string gameSaveFilePath = data_path() + "/save/game_save.json";
	// Map to store group of components for each entity
	std::map<int, std::map<std::string, json>> entityComponentGroups;

	// Serialization
	void serialize_containers(json& j);

	template <typename Component>
	json serialize_container(const ComponentContainer<Component>& container);
	json serialize_mesh_container(const ComponentContainer<Mesh*>& container);

	nlohmann::json serialize_game_timer(const GameTimer& gameTimer);
	nlohmann::json serialize_game_score(const GameScore& gameScore);

	template <typename Component>
	nlohmann::json serialize_component(const Component& component);

	// Deserialization
	void groupComponentsForEntities(const json& j);

	void deserialize_containers(const json& j);
	void createEntity(std::vector<std::string> componentNames, std::map<std::string, nlohmann::json> componentsMap);
	void deserialize_game_timer(const json& j);
	void deserialize_game_score(const json& j);

	void createPlayerDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createBoarDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createBarbarianDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createArcherDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createBirdFlockDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createWizardDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTrollDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createHeartDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createCollectibleTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTrapDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTreeDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createObstacleDeserialization(std::map<std::string, nlohmann::json> componentsMap);
	void createTargetAreaDeserialization(std::map<std::string, nlohmann::json> componentsMap);
};