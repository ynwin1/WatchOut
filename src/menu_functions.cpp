#pragma once

#include "world_system.hpp"

Entity WorldSystem::createHelpMenu(vec2 cameraPosition) {
	auto entity = Entity();

	registry.pauseMenuComponents.emplace(entity);

	registry.foregrounds.emplace(entity);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = {cameraPosition, 0};
	motion.angle = 0.f;
	motion.scale = { 960, 540 };

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

Entity WorldSystem::createPauseMenu(vec2 cameraPosition) {
	auto entity = Entity();

	registry.pauseMenuComponents.emplace(entity);

	registry.foregrounds.emplace(entity);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = {cameraPosition, 0};
	motion.angle = 0.f;
	motion.scale = { 960, 540 };

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