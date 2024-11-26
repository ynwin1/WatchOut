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

//Tutorial at the start
Entity WorldSystem::createTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.tutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, windowSize.y / 2};
    fg.scale = {960, 540};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::TUTORIAL_1,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

void WorldSystem::exitTutorial() {
	for (auto& entity: registry.tutorialComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}

Entity WorldSystem::createBoarTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BOAR_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}
Entity WorldSystem::createBirdTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BIRD_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}
Entity WorldSystem::createWizardTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::WIZARD_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}
Entity WorldSystem::createTrollTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::TROLL_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}
Entity WorldSystem::createArcherTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::ARCHER_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}
Entity WorldSystem::createBarbarianTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BARBARIAN_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

void WorldSystem::exitEnemyTutorial() {
	for (auto& entity: registry.enemyTutorialComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}

Entity WorldSystem::createHeartTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.collectibleTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::HEART_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

Entity WorldSystem::createTrapTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.collectibleTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 600};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::TRAP_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

void WorldSystem::exitCollectibleTutorial() {
	for (auto& entity: registry.collectibleTutorialComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}