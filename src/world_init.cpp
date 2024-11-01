#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <random>

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
	enemy.cooldown = 1500.f; // 1.5s
	enemy.speed = BOAR_SPEED;

	registry.boars.emplace(entity);

	// Add Render Request for drawing sprite
	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::BOAR,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
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
	motion.scale = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT };
	motion.hitbox = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT / zConversionFactor };
	motion.solid = true;
	
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 30;
	enemy.cooldown = 2000.f; // 2s
	enemy.speed = BARBARIAN_SPEED;

	registry.barbarians.emplace(entity);

	// Add Render Request for drawing sprite
	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::BARBARIAN,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
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

	// Add Render Request for drawing sprite
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 40;
	enemy.cooldown = 3000.f; // 3s
	enemy.speed = ARCHER_SPEED;

	registry.archers.emplace(entity);

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::ARCHER,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
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

	//Initialize stamina
	auto& stamina = registry.staminas.emplace(entity);

	//Initialize movement
	auto& player = registry.players.emplace(entity);
	player.isRolling = false;
	player.isRunning = false;
	player.facing = { 1, 0 };

	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	auto& jumper = registry.jumpers.emplace(entity);
	jumper.speed = 2;

	motion.scale = vec2({ JEFF_BB_WIDTH, JEFF_BB_HEIGHT });
	motion.hitbox = { JEFF_BB_WIDTH, JEFF_BB_WIDTH, JEFF_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	// Jeff Render Request
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::JEFF,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec3(0.0f, 1.0f, 0.0f));
	createStaminaBar(entity, vec3(0.0f, 0.0f, 1.0f));
	
	
	return entity;
}

Entity createTree(RenderSystem* renderer, vec2 pos)
{
	bool RENDERING_MESH = false;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TREE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, 0);
	motion.angle = 0.f;
	motion.scale = { MESHTREE_BB_WIDTH, MESHTREE_BB_HEIGHT };
	motion.hitbox = { MESHTREE_BB_WIDTH, MESHTREE_BB_WIDTH, MESHTREE_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	// print tree position
	printf("Tree position: %f, %f, %f\n", pos.x, pos.y, motion.position.z);

	if (!RENDERING_MESH) {
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
	}

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

Entity createGameOver(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, 0);
	motion.angle = 0.f;
	motion.scale = { GO_BB_WIDTH, GO_BB_HEIGHT };

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::GAMEOVER,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
	registry.foregrounds.emplace(entity);
	
	return entity;
};

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

Entity createFPSText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00 fps";
	// text position based on screen coordinates
	text.position = {windowSize.x - 60.0f, windowSize.y - 20.0f};
	text.scale = 0.5f;

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

	// int numTrees = 15;
	// while(numTrees != 0) {
    // 	float posX = (uniform_dist(rng) * world_size_x);
	// 	float posY = (uniform_dist(rng) * world_size_y);
	// 	createObstacle({posX, posY}, {150.f, 250.f}, TEXTURE_ASSET_ID::TREE);
	// 	numTrees--;
    // }
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
	int numTrees = 5;
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

void createMapTiles(GLFWwindow* window) {
    int windowWidth;
    int windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

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

void createMeshTrees(RenderSystem* renderer) {
	int numTrees = 3;
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
