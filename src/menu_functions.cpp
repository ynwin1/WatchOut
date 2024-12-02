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
    fg.position = {windowSize.x / 2, windowSize.y / 2 - 50};
    fg.scale = {1500, 1200};
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
    fg.scale = {900, 700};
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
    fg.scale = {900, 700};
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
    fg.scale = {900, 700};
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
    fg.scale = {900, 700};
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
    fg.scale = {900, 700};
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
    fg.scale = {900, 700};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BARBARIAN_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

Entity WorldSystem::createBomberTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.enemyTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 700};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BOMBER_INTRO,
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
    fg.scale = {900, 700};
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
    fg.scale = {900, 700};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::TRAP_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

Entity WorldSystem::createPhantomTrapTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.collectibleTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 700};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::PHANTOM_TRAP_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

Entity WorldSystem::createBowTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.collectibleTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 700};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BOW_INTRO,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }); 

    return entity;
}

Entity WorldSystem::createBombTutorial(vec2 windowSize) {
    auto entity = Entity();
    registry.collectibleTutorialComponents.emplace(entity);

    Foreground& fg = registry.foregrounds.emplace(entity);
    fg.position = {windowSize.x / 2, 100};
    fg.scale = {900, 700};
    registry.renderRequests.insert(
        entity,
        {
            TEXTURE_ASSET_ID::BOMB_INTRO,
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

void WorldSystem::createTitleScreen() {
	registry.clear_all_components();

	vec2 windowSize = camera->getSize();
	createTitleScreenBackground(windowSize);
	// createTitleScreenText(windowSize, "Watch Out!", 5.f, vec2(windowSize.x / 2 - 400.f, windowSize.y / 2 + 100));
	createTitleScreenTitle(windowSize);
	createTitleScreenText(windowSize, "Press Enter to Begin", 1.f, vec2(windowSize.x / 2 - 155.f, windowSize.y / 2 - 100));
	createTitleScreenText(windowSize, "Press L to Load Progress", 1.f, vec2(windowSize.x / 2 - 185.f, windowSize.y / 2 - 200));
	createTitleScreenText(windowSize, "Press Q to Quit", 1.f, vec2(windowSize.x / 2 - 115.f, windowSize.y / 2 - 300));
	std::string teamText = "Team 17 Electric Boogaloo: Carlo, Katie, Linus, Tarun & Yan Naing";
	createTitleScreenText(windowSize, teamText, 1.f, vec2(windowSize.x - 940.f, windowSize.y - 50));
	camera->followPosition({ world_size_x / 2.f, world_size_y / 2.f });

	soundSetUp();
	gameStateController.setGameState(GAME_STATE::TITLE);
}