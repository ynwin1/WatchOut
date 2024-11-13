#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "animation_system.hpp"
#include "animation_system_init.hpp"
#include <random>
#include <sstream>

// Boar creation
Entity createBoar(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + BOAR_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT };
	motion.hitbox = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT, BOAR_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 20;
	enemy.speed = BOAR_SPEED;

	registry.boars.emplace(entity);
	
	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	initBoarAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec3(1.0f, 0.0f, 0.0f));
	
	return entity;
};

// Barbarian creation
Entity createBarbarian(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + BARBARIAN_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { 32. * SPRITE_SCALE, 36. * SPRITE_SCALE};
	motion.hitbox = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT / zConversionFactor };
	motion.solid = true;
	
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 30;
	enemy.cooldown = 1000;
	enemy.speed = BARBARIAN_SPEED;

	registry.barbarians.emplace(entity);

	initBarbarianAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec3(1.0f, 0.0f, 0.0f));

	return entity;
};

// Archer creation
Entity createArcher(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + ARCHER_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT };
	motion.hitbox = { ARCHER_BB_WIDTH, ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 40;
	enemy.speed = ARCHER_SPEED;

	registry.archers.emplace(entity);

	initArcherAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec3(1.0f, 0.0f, 0.0f));
	
	return entity;
};

// Collectible trap creation
Entity createCollectibleTrap(vec2 pos)
{
	auto entity = Entity();
	registry.collectibleTraps.emplace(entity);

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TRAP_COLLECTABLE_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT };
	motion.hitbox = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT / zConversionFactor };

	registry.collectibles.emplace(entity);

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::TRAPCOLLECTABLE,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});

	registry.midgrounds.emplace(entity);

	printf("Collectible trap created\n");

	return entity;
};

// Heart creation
Entity createHeart(vec2 pos)
{
	auto entity = Entity();
	registry.hearts.emplace(entity);

	// Setting intial motion values
	Motion& fixed = registry.motions.emplace(entity);
	fixed.position = vec3(pos, getElevation(pos) + HEART_BB_WIDTH / 2);
	fixed.angle = 0.f;
	fixed.scale = { HEART_BB_WIDTH, HEART_BB_WIDTH };
	fixed.hitbox = { HEART_BB_WIDTH, HEART_BB_WIDTH, HEART_BB_HEIGHT / zConversionFactor };

	registry.collectibles.emplace(entity);

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::HEART,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});

	registry.midgrounds.emplace(entity);

	return entity;
};

// Damage trap creation
Entity createDamageTrap(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TRAP_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };
	motion.hitbox = { TRAP_BB_WIDTH, TRAP_BB_WIDTH, TRAP_BB_HEIGHT / zConversionFactor };

	// Setting initial trap values
	registry.traps.emplace(entity);

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::TRAP,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});

	registry.midgrounds.emplace(entity);

	return entity;
};

// Create Player Jeff
Entity createJeff(vec2 position)
{
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = vec3(position, getElevation(position) + JEFF_BB_HEIGHT / 2);
	motion.facing = { 1, 0 };

	//Initialize stamina
	auto& stamina = registry.staminas.emplace(entity);

	//Initialize movement
	auto& player = registry.players.emplace(entity);
	player.isRolling = false;
	player.isRunning = false;

	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 32. * SPRITE_SCALE, 32. * SPRITE_SCALE});
	motion.hitbox = { JEFF_BB_WIDTH, JEFF_BB_WIDTH, JEFF_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	auto& jumper = registry.jumpers.emplace(entity);
	jumper.speed = 2;

	// Animation
	initJeffAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec3(0.0f, 1.0f, 0.0f));
	createStaminaBar(entity, vec3(0.0f, 0.0f, 1.0f));
	
	
	return entity;
}

Entity createTree(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TREE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, 0);
	motion.angle = 0.f;
	motion.scale = { TREE_BB_WIDTH, TREE_BB_HEIGHT };
	motion.hitbox = { TREE_BB_WIDTH, TREE_BB_WIDTH, TREE_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	// print tree position
	printf("Tree position: %f, %f, %f\n", pos.x, pos.y, motion.position.z);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TREE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	/*if (!RENDERING_MESH) {
		registry.renderRequests.insert(
			entity, {
				TEXTURE_ASSET_ID::TREE,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity, {
				TEXTURE_ASSET_ID::TREE,
				EFFECT_ASSET_ID::TREE,
				GEOMETRY_BUFFER_ID::TREE });
	}*/

	registry.obstacles.emplace(entity);
	registry.midgrounds.emplace(entity);

	return entity;
}

Entity createArrow(vec3 pos, vec3 velocity)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = velocity;
	motion.scale = { ARROW_BB_WIDTH, ARROW_BB_HEIGHT };
	motion.hitbox = { ARROW_BB_WIDTH, ARROW_BB_HEIGHT, ARROW_BB_HEIGHT / zConversionFactor };
	
	registry.projectiles.emplace(entity);
	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.damage = 50;
	registry.midgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

void createHealthBar(Entity characterEntity, vec3 color) {
	auto meshEntity = Entity();

	const float width = 60.0f;
	const float height = 10.0f;

	Motion& characterMotion = registry.motions.get(characterEntity);

	Motion& motion = registry.motions.emplace(meshEntity);
	// position does not need to be initialized as it will always be set to match the associated entity
	motion.angle = 0.f;
	motion.scale = { width, height };

	registry.colours.insert(meshEntity, color);

	registry.renderRequests.insert(
		meshEntity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::HEALTH_BAR
		});
	registry.midgrounds.emplace(meshEntity);

	HealthBar& hpbar = registry.healthBars.emplace(characterEntity, meshEntity);
	hpbar.width = width;
	hpbar.height = height;
}

void createStaminaBar(Entity characterEntity, vec3 color) {
	auto meshEntity = Entity();

	const float width = 60.0f;
	const float height = 10.0f;

	Motion& characterMotion = registry.motions.get(characterEntity);

	Motion& motion = registry.motions.emplace(meshEntity);
	motion.angle = 0.f;
	motion.scale = { width, height };


	registry.colours.insert(meshEntity, color);

	registry.renderRequests.insert(
		meshEntity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::STAMINA_BAR
		});
	registry.midgrounds.emplace(meshEntity);

	StaminaBar& staminabar = registry.staminaBars.emplace(characterEntity, meshEntity);
	staminabar.width = width;
	staminabar.height = height;
}

Entity createPauseHelpText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "PAUSE/PLAY(P)    HELP (H)";
	text.position = {windowSize.x - 550, windowSize.y - 70.0f};
	text.scale = 1.5f;

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createPauseMenu(vec2 cameraPosition) {
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


void exitPauseMenu() {
	for (auto& entity: registry.pauseMenuComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}

Entity createHelpMenu(vec2 cameraPosition) {
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

void exitHelpMenu() {
	for (auto& entity: registry.pauseMenuComponents.entities) {
		registry.remove_all_components_of(entity);
	}
}

Entity createFPSText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00 fps";
	// text position based on screen coordinates
	text.position = {90.0f, windowSize.y - 40.0f};
	text.scale = 0.8f;

	registry.renderRequests.insert(
			entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createGameTimerText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00:00:00";
	text.position = {(windowSize.x / 2) + 50.0f, windowSize.y - 80.0f}; 
	text.scale = 2.0f;

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createTrapsCounterText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "Traps: 00";
	text.position = {(windowSize.x / 2) - 250.0f, windowSize.y - 80.0f}; 
	text.scale = 1.5f;

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});
	
	return entity;
}

Entity createMapTile(vec2 position, vec2 size) {
    auto entity = Entity();

    MapTile& tile = registry.mapTiles.emplace(entity);
    tile.position = position;
    tile.scale = size;
	
    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::GRASS_TILE,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::MAP_TILE
        });
    registry.backgrounds.emplace(entity);
	
    return entity;
}

void createObstacles() {
	std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist;
    rng = std::default_random_engine(std::random_device()());

	int numShrubs = 20;
	while(numShrubs != 0) {
    	float posX = (uniform_dist(rng) * world_size_x);
		float posY = (uniform_dist(rng) * world_size_y);
		createObstacle({posX, posY}, {SHRUB_BB_WIDTH, SHRUB_BB_HEIGHT}, TEXTURE_ASSET_ID::SHRUB);
		numShrubs--;
    }
	int numRocks = 15;
	while(numRocks != 0) {
    	float posX = (uniform_dist(rng) * world_size_x);
		float posY = (uniform_dist(rng) * world_size_y);
		createObstacle({posX, posY}, {ROCK_BB_WIDTH, ROCK_BB_HEIGHT}, TEXTURE_ASSET_ID::ROCK);
		numRocks--;
    }
}

Entity createObstacle(vec2 position, vec2 size, TEXTURE_ASSET_ID assetId) {
    auto entity = Entity();
    registry.obstacles.emplace(entity);

    Motion& motion = registry.motions.emplace(entity);
    motion.scale = size;

    motion.position = vec3(position.x, position.y, getElevation(position) + size.y / 2);
	motion.hitbox = { size.x, size.x, size.y / zConversionFactor };
	motion.solid = true;

    registry.renderRequests.insert(
        entity, 
        {
            assetId,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.midgrounds.emplace(entity);

    return entity;
}


Entity createBottomCliff(vec2 position, vec2 size) {
    auto entity = Entity();
	MapTile& tile = registry.mapTiles.emplace(entity);
    tile.position = position;
    tile.scale = size;

    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::CLIFF,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.backgrounds.emplace(entity); 
    return entity;
}

Entity createSideCliff(vec2 position, vec2 size) {
    auto entity = Entity();
	MapTile& tile = registry.mapTiles.emplace(entity);
    tile.position = position;
    tile.scale = size;

    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::CLIFFSIDE,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.backgrounds.emplace(entity); 
    return entity;
}
Entity createTopCliff(vec2 position, vec2 size) {
    auto entity = Entity();
	MapTile& tile = registry.mapTiles.emplace(entity);
    tile.position = position;
    tile.scale = size;

    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::CLIFFTOP,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.backgrounds.emplace(entity); 
    return entity;
}

void createCliffs(GLFWwindow* window) {
    int windowWidth;
    int windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int cliffsOnScreenX = 6; 
    int cliffsOnScreenY = 6; 
    float cliffWidth = 500;
	float bottomCliffWidth = 480;
    float cliffHeight = 500;
	float sideCliffHeight = 510;
	float bottomCliffOffset = 70;

    float cliffThickness = 500.0f;
	float sideCliffThickness = 510.0f;


	// Top boundary cliffs
    for (int col = 0; col <= cliffsOnScreenX; col++) {
        vec2 position = {leftBound + col * cliffWidth, topBound - cliffThickness}; 
        vec2 size = {cliffWidth, cliffThickness};
        createTopCliff(position, size);
    }
	// Bottom boundary cliffs
    for (int col = 0; col <= cliffsOnScreenX; col++) {
        vec2 position = {leftBound + bottomCliffWidth / 2 + col * bottomCliffWidth - bottomCliffOffset, bottomBound - cliffThickness};  
        vec2 size = {cliffWidth, cliffThickness};
        createBottomCliff(position, size);
    }
    // Left boundary cliffs
    for (int row = 0; row < cliffsOnScreenY - 2; row++) {
        vec2 position = {leftBound - cliffThickness / 2, row * sideCliffHeight}; 
        vec2 size = {sideCliffHeight, sideCliffThickness};
        createSideCliff(position, size);
    }

    // Right boundary cliffs
    for (int row = 0; row < cliffsOnScreenY - 2; row++) {
        vec2 position = {rightBound + cliffThickness / 2,  row * sideCliffHeight};
        vec2 size = {-sideCliffHeight, sideCliffThickness};
        createSideCliff(position, size);
    }
}

void createMapTiles() {

    int tilesOnScreenX = 10; 
    int tilesOnScreenY = 6; 
    float tileWidth = world_size_x / tilesOnScreenX;
    float tileHeight = world_size_y / tilesOnScreenY;
    int numRows = tilesOnScreenY;
    int numCols = tilesOnScreenX;

    for (int row = 0; row < numRows; row++) { 
        for (int col = 0; col < numCols; col++) { 
            vec2 position = {col * tileWidth, row * tileHeight};
            vec2 size = {tileWidth, tileHeight};
            createMapTile(position, size);
        }
    }
}

void createGameOverText(vec2 windowSize) {
	GameTimer& gameTimer = registry.gameTimer;
	std::vector<Entity> entities;

	auto entity1 = Entity();
	Text& text1 = registry.texts.emplace(entity1);
	text1.value = "GAME OVER";
	text1.position = {windowSize.x / 2 - 315.0f, windowSize.y / 2 + 50.0f};
	text1.scale = 4.0f;
	text1.colour = {0.85f, 0.0f, 0.0f};
	
	auto entity2 = Entity();
	Text& text2 = registry.texts.emplace(entity2);
	text2.position = {windowSize.x / 2 - 160.f, windowSize.y / 2};
	text2.scale = 1.0f;
	text2.colour = {1.0f, 0.85f, 0.0f};
	std::ostringstream oss;
    oss << "You survived for ";
    if (gameTimer.hours > 0) {
        oss << gameTimer.hours << "h ";
		text2.position.x -= 20.f;
    }
    if (gameTimer.minutes > 0 || gameTimer.hours > 0) {
        oss << gameTimer.minutes << "m ";
		text2.position.x -= 40.f;
    }
    oss << gameTimer.seconds << "s";
	text2.value = oss.str();

	auto entity3 = Entity();
	Text& text3 = registry.texts.emplace(entity3);
	text3.position = {windowSize.x / 2 - 165.f, windowSize.y / 2 - 150.f};
	text3.scale = 0.8f;
	text3.value = "Press ENTER to play again";

	entities.push_back(entity1);
	entities.push_back(entity2);
	entities.push_back(entity3);
	for (Entity& entity : entities) {
		registry.renderRequests.insert(
			entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});
	}
}

Entity createHighScoreText(vec2 windowSize, int hours, int minutes, int seconds){
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.position = {windowSize.x / 2 - 165.f, windowSize.y / 2 - 80.f};
	text.scale = 0.8f;
	text.colour = {1.0f, 0.85f, 0.0f};
    text.value = "Your High Score is  " + std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s ";

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

void createTrees(RenderSystem* renderer) {
	int numTrees = 4;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	rng = std::default_random_engine(std::random_device()());

	while (numTrees != 0) {
		float posX = (uniform_dist(rng) * world_size_x);
		float posY = (uniform_dist(rng) * world_size_y);
		createTree(renderer, { posX, posY });
		numTrees--;
	}
}

float getElevation(vec2 xy)
{
	return 0.0f;
}
