#pragma once

#include "world_system.hpp"

Entity WorldSystem::createHelpMenu(vec2 windowSize) {
	auto entity = Entity();

	registry.pauseMenuComponents.emplace(entity);

	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {windowSize.x / 2, windowSize.y / 2};
	fg.scale = { 960, 540 };

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::MENU_HELP,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

void WorldSystem::exitHelpMenu() {
	for (auto& entity: registry.pauseMenuComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}

Entity WorldSystem::createPauseMenu(vec2 windowSize) {
	auto entity = Entity();

	registry.pauseMenuComponents.emplace(entity);

	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {windowSize.x / 2, windowSize.y / 2};
	fg.scale = { 960, 540 };

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::MENU_PAUSED,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}


void WorldSystem::exitPauseMenu() {
	for (auto& entity: registry.pauseMenuComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}

void WorldSystem::createTitleScreen() {
	registry.clear_all_components();

	vec2 windowSize = camera->getSize();
	createTitleScreenBackground(windowSize);
	createTitleScreenText(windowSize, "Watch Out!", 5.f, vec2(windowSize.x / 2 - 400.f, windowSize.y / 2 + 100));
	createTitleScreenText(windowSize, "Press Enter to Begin", 1.f, vec2(windowSize.x / 2 - 155.f, windowSize.y / 2 - 100));
	camera->followPosition({ world_size_x / 2.f, world_size_y / 2.f });

	soundSetUp();
	gameStateController.setGameState(GAME_STATE::TITLE);
}