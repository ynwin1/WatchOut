#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "animation_system.hpp"
#include "animation_system_init.hpp"
#include <random>
#include <sstream>
#include "render_system.hpp"

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
	enemy.damage = BOAR_DAMAGE;
	enemy.maxHealth = BOAR_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = BOAR_SPEED;

	registry.boars.emplace(entity);
	
	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	initBoarAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	registry.knockers.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = BOAR_SPEED;
	
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
	enemy.damage = BARBARIAN_DAMAGE;
	enemy.cooldown = 1000;
	enemy.maxHealth = BARBARIAN_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = BARBARIAN_SPEED;

	registry.barbarians.emplace(entity);

	initBarbarianAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	registry.knockers.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = BARBARIAN_SPEED;

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
	enemy.damage = ARCHER_DAMAGE;
	enemy.maxHealth = ARCHER_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = ARCHER_SPEED;

	registry.archers.emplace(entity);

	initArcherAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = ARCHER_SPEED;
	
	return entity;
};

// Bird creation (Flock of 5 birds)
Entity createBirdFlock(vec2 pos)
{
    const int flockSize = 5;
    const float spacing = 20.f; 
    Entity repBird;

    for (int i = 0; i < flockSize; ++i)
    {
        auto entity = Entity();

        // Spawn birds with spacing
        vec2 birdPosition = pos + vec2(i * spacing, 0); 

        Motion& motion = registry.motions.emplace(entity);
        motion.position = vec3(birdPosition, TREE_BB_HEIGHT - BIRD_BB_WIDTH);
        motion.angle = 0.f;
        motion.scale = { 16 * SPRITE_SCALE, 16 * SPRITE_SCALE};
        motion.hitbox = { BIRD_BB_WIDTH, BIRD_BB_HEIGHT, BIRD_BB_HEIGHT / zConversionFactor };
        motion.solid = true;

        Enemy& enemy = registry.enemies.emplace(entity);
        enemy.damage = BIRD_DAMAGE;
        enemy.cooldown = 2000.f;
		enemy.maxHealth = BIRD_HEALTH;
		enemy.health = enemy.maxHealth;
        motion.speed = BIRD_SPEED;

        registry.birds.emplace(entity);

        initBirdAnimationController(entity);
        registry.midgrounds.emplace(entity);

        createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (i == 0)
        {
            repBird = entity;
        }

		registry.knockables.emplace(entity);
		auto& trappable = registry.trappables.emplace(entity);
		trappable.originalSpeed = BIRD_SPEED;
    }
    return repBird;
}


// Wizard creation
Entity createWizard(vec2 pos) {
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + WIZARD_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { 96 * SPRITE_SCALE,  35 * SPRITE_SCALE };
	motion.hitbox = { WIZARD_BB_WIDTH, WIZARD_BB_WIDTH, WIZARD_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = WIZARD_DAMAGE;
	enemy.cooldown = 8000.f; // 8s
	enemy.maxHealth = WIZARD_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = WIZARD_SPEED;

	registry.wizards.emplace(entity);

	initWizardAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = WIZARD_SPEED;

	return entity;
}

Entity createTroll(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TROLL_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TROLL_BB_WIDTH, TROLL_BB_HEIGHT };
	motion.hitbox = { TROLL_BB_WIDTH * 0.9, TROLL_BB_WIDTH * 0.9, TROLL_BB_HEIGHT * 0.9 / zConversionFactor };
	motion.solid = true;
	if (registry.players.entities.size() > 0) {
		vec2 playerPosition = vec2(registry.motions.get(registry.players.entities.at(0)).position);
		motion.facing = normalize(playerPosition - pos);
	}

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = TROLL_DAMAGE;
	enemy.cooldown = 0;
	motion.speed = TROLL_SPEED;
	enemy.maxHealth = TROLL_HEALTH;
	enemy.health = enemy.maxHealth;

	registry.trolls.emplace(entity);

	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = TROLL_SPEED;
	Knocker& knocker = registry.knockers.emplace(entity);
	knocker.strength = 1.5f;

	initTrollAnimationController(entity);

	return entity;
}
;

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

	initTrapBottleAnimationController(entity);

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

	initHeartAnimationController(entity);

	registry.midgrounds.emplace(entity);

	return entity;
};

Entity createCollected(Motion& playerM, vec2 size, TEXTURE_ASSET_ID assetID)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = { size.x * 0.7, size.y * 0.7};

	registry.collected.emplace(entity);
	registry.midgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

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

	registry.backgrounds.emplace(entity);

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
	motion.speed = PLAYER_SPEED;

	auto& jumper = registry.jumpers.emplace(entity);
	jumper.speed = 2;

	// Animation
	initJeffAnimationController(entity);
	registry.midgrounds.emplace(entity);

	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = PLAYER_SPEED;
	
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

Entity createArrow(vec3 pos, vec3 velocity, int damage)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = velocity;
	motion.scale = { ARROW_BB_WIDTH, ARROW_BB_HEIGHT };
	motion.hitbox = { ARROW_BB_WIDTH, ARROW_BB_HEIGHT, ARROW_BB_HEIGHT / zConversionFactor };
	
	registry.projectiles.emplace(entity);
	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.damage = damage;
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

Entity createFireball(vec3 pos, vec2 direction) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = vec3(0);
	motion.angle = atan2(direction.y, direction.x);
	motion.scale = { FIREBALL_BB_WIDTH, FIREBALL_BB_HEIGHT };
	motion.hitbox = { FIREBALL_HITBOX_WIDTH, FIREBALL_HITBOX_WIDTH, FIREBALL_HITBOX_WIDTH };

	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.type = "fireball";
	damaging.damage = 30;
	registry.midgrounds.emplace(entity);

	initFireballAnimationController(entity);
	return entity;
}

Entity createLightning(vec2 pos) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	
	// add half the hitbox size to the vec2 pos
	motion.scale = { LIGHTNING_BB_WIDTH, LIGHTNING_BB_HEIGHT };
	motion.hitbox = { LIGHTNING_BB_WIDTH, LIGHTNING_BB_WIDTH, LIGHTNING_BB_HEIGHT / zConversionFactor };
	motion.position = vec3(pos, motion.hitbox.z / 2);

	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.type = "lightning";
	damaging.damage = 20;
	registry.midgrounds.emplace(entity);

	Cooldown& duration = registry.cooldowns.emplace(entity);
	duration.remaining = 1500.f;

	initLightningAnimationController(entity);
	return entity;
}

void createPlayerStaminaBar(Entity characterEntity, vec2 windowSize) {
	auto meshE = Entity();
	const float width = 150.0f;
	const float height = 20.0f;

	vec2 position = {210.0f, windowSize.y - 80.0f};

	Foreground& fg = registry.foregrounds.emplace(meshE);
	fg.position = position;
	fg.scale = { width, height };

	vec4 colour = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	registry.colours.insert(meshE, colour);

	registry.renderRequests.insert(
		meshE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE
		});

	// Bar frame
	auto frameE = Entity();
	Foreground& frameFg = registry.foregrounds.emplace(frameE);
	frameFg.position = position;
	frameFg.scale = { width, height };
	registry.colours.insert(frameE, colour);
	registry.renderRequests.insert(
		frameE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			PRIMITIVE_TYPE::LINES,
		});

	auto textE = Entity();
	registry.texts.emplace(textE);
	Foreground& textFg = registry.foregrounds.emplace(textE);
	textFg.scale = {0.8f, 0.8f};
	textFg.position = {position.x - 100.0f, position.y};
	registry.renderRequests.insert(
		textE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT,
		});

	StaminaBar& staminabar = registry.staminaBars.emplace(characterEntity, meshE, frameE);
	staminabar.width = width;
	staminabar.height = height;
	staminabar.textEntity = textE;
}

void createPlayerHealthBar(Entity characterEntity, vec2 windowSize) {
	auto meshE= Entity();
	const float width = 150.0f;
	const float height = 20.0f;

	vec2 position = {210.0f, windowSize.y - 50.0f};

	Foreground& fg = registry.foregrounds.emplace(meshE);
	fg.position = position;
	fg.scale = {width, height};

	vec4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	registry.colours.insert(meshE, green);

	registry.renderRequests.insert(
		meshE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
		});

	// Bar frame
	auto frameE = Entity();
	Foreground& frameFg = registry.foregrounds.emplace(frameE);
	frameFg.position = position;
	frameFg.scale = { width, height };
	registry.colours.insert(frameE, green);
	registry.renderRequests.insert(
		frameE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			PRIMITIVE_TYPE::LINES,
		});

	auto textE = Entity();
	registry.texts.emplace(textE);
	Foreground& textFg = registry.foregrounds.emplace(textE);
	textFg.scale = {0.8f, 0.8f};
	textFg.position = {position.x - 32.0f, position.y};
	registry.renderRequests.insert(
		textE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT,
		});
	HealthBar& hpbar = registry.healthBars.emplace(characterEntity, meshE, frameE);
	hpbar.width = width;
	hpbar.height = height;
	hpbar.textEntity = textE;
}

void createHealthBar(Entity characterEntity, vec4 color) {
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
			GEOMETRY_BUFFER_ID::RECTANGLE,
		});
	registry.midgrounds.emplace(meshEntity);

	// HP bar frame
	auto frameEntity = Entity();
	Motion& frameM = registry.motions.emplace(frameEntity);
	frameM.position = motion.position;
	frameM.scale = { width, height };
	registry.colours.insert(frameEntity, color);
	registry.renderRequests.insert(
		frameEntity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			PRIMITIVE_TYPE::LINES,
		});
	registry.midgrounds.emplace(frameEntity);

	HealthBar& hpbar = registry.healthBars.emplace(characterEntity, meshEntity, frameEntity);
	hpbar.width = width;
	hpbar.height = height;
}

Entity createTargetArea(vec3 position, float radius) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.position.z = 0.f;

	float ogRadius = 170.f;
	float scaledFactor = radius / ogRadius;
	motion.scale = { 2 * ogRadius * scaledFactor, 2 * ogRadius * scaledFactor * zConversionFactor };

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TARGET_AREA,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	registry.backgrounds.emplace(entity);
	registry.targetAreas.emplace(entity);
	Cooldown& cooldown = registry.cooldowns.emplace(entity);
	cooldown.remaining = 3000.f; // 5s

	printf("Target area created\n");
	return entity;
}

Entity createPauseHelpText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "PAUSE/PLAY(P)    HELP (H)";
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {windowSize.x - 550, windowSize.y - 70.0f};
	fg.scale = {1.5f, 1.5f};

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createFPSText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00 fps";
	// text position based on screen coordinates
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {30.0f, windowSize.y - 40.0f};
	fg.scale = {0.8f, 0.8f};

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
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {(windowSize.x / 2) + 120.0f, windowSize.y - 70.0f};
	fg.scale = {1.5f, 1.5f};

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
	auto textE = Entity();

	vec2 position = {(windowSize.x / 2) - 500.0f, windowSize.y - 80.0f};

	registry.texts.emplace(textE);
	Foreground& fg = registry.foregrounds.emplace(textE);
	fg.position = position;
	fg.scale = {1.5f, 1.5f};

	registry.colours.insert(textE, {0.8f, 0.8f, 0.0f, 1.0f});

	registry.renderRequests.insert(
		textE, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	auto iconE = Entity();

	Foreground& icon = registry.foregrounds.emplace(iconE);
	icon.scale = { TRAP_COLLECTABLE_BB_WIDTH * 1.25, TRAP_COLLECTABLE_BB_WIDTH * 1.25 };
	icon.position = {position.x - 20.0f, position.y + 16.0f};

	registry.renderRequests.insert(
		iconE, 
		{
			TEXTURE_ASSET_ID::TRAPCOLLECTABLE,
			EFFECT_ASSET_ID::TEXTURED_FLAT,
			GEOMETRY_BUFFER_ID::SPRITE
		});
	
	return textE;
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
		float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
		float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
		createObstacle({posX, posY}, {SHRUB_BB_WIDTH, SHRUB_BB_HEIGHT}, TEXTURE_ASSET_ID::SHRUB);
		numShrubs--;
    }
	int numRocks = 15;
	while(numRocks != 0) {
		float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
		float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
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

void createGameOverText(vec2 windowSize, GameStateController& gameStateController) {
	auto backdrop = Entity();
	Foreground& backdropFg = registry.foregrounds.emplace(backdrop);
	backdropFg.position = {0.0f, 0.0f};
	backdropFg.scale = {windowSize.x, windowSize.y};

	registry.colours.insert(backdrop, vec4(0.0f, 0.0f, 0.0f, 0.6f));
	registry.renderRequests.insert(
		backdrop,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE
		});

	std::vector<Entity> entities;
	GameTimer& gameTimer = gameStateController.gameTimer;
	GameScore& gameScore = gameStateController.gameScore;

	auto entity1 = Entity();
	Text& text1 = registry.texts.emplace(entity1);
	Foreground& text1Fg = registry.foregrounds.emplace(entity1);
	text1.value = "GAME OVER";
	text1Fg.position = {windowSize.x / 2 - 315.0f, windowSize.y / 2 + 50.0f};
	text1Fg.scale = {4.0f, 4.0f};
	registry.colours.insert(entity1, {0.85f, 0.0f, 0.0f, 1.0f});

	auto entity2 = Entity();
	Text& text2 = registry.texts.emplace(entity2);
	text2.lineSpacing = 1.5f;
	Foreground& text2Fg = registry.foregrounds.emplace(entity2);
	text2Fg.position = {windowSize.x / 2 - 160.f, windowSize.y / 2 - 20.f};
	text2Fg.scale = {1.f, 1.f};
	registry.colours.insert(entity2, {1.0f, 0.85f, 0.0f, 1.0f});

	std::ostringstream oss;
    oss << "You survived for ";
    if (gameTimer.hours > 0) {
        oss << gameTimer.hours << "h ";
    }
    if (gameTimer.minutes > 0 || gameTimer.hours > 0) {
        oss << gameTimer.minutes << "m ";
    }
    oss << gameTimer.seconds << "s";
	text2.value = oss.str();

	oss.str("");
	oss << "\nYour Score: " << gameScore.score;
	text2.value += oss.str();

	oss.str("");
	oss << "\nYour High Score: " << gameScore.highScore;
	text2.value += oss.str();

	auto entity3 = Entity();
	Text& text3 = registry.texts.emplace(entity3);
	text3.value = "Press ENTER to play again";
	Foreground& text3Fg = registry.foregrounds.emplace(entity3);
	text3Fg.position = {windowSize.x / 2 - 200.f, windowSize.y / 2 - 170.f};
	text3Fg.scale = {1.f, 1.f};

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

void createTrees(RenderSystem* renderer) {
	int numTrees = 4;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	rng = std::default_random_engine(std::random_device()());

	while (numTrees != 0) {
		float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
		float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
		createTree(renderer, { posX, posY });
		numTrees--;
	}
}

Entity createPointsEarnedText(std::string textValue, Entity anchoredWorldEntity, vec4 color, float isSuvivalBonus) {
	auto entity = Entity();
	Motion& anchoredMotion = registry.motions.get(anchoredWorldEntity);
	Text& text = registry.texts.emplace(entity);
	text.value = textValue;
	text.anchoredWorldEntity = anchoredWorldEntity;
	text.anchoredWorldOffset = {-70.0f, -anchoredMotion.scale.y / 2 - 20.0f};
	if(isSuvivalBonus) {
		text.anchoredWorldOffset.x = -120.0f;
	}

	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.scale = {1.0f, 1.0f};

	registry.colours.insert(entity, color);

	SlideUp& slideUp = registry.slideUps.emplace(entity);
	slideUp.fadeIn = true;

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createComboText(int comboValue, vec2 windowSize) {
	auto entity = Entity();
	Text& text = registry.texts.emplace(entity);
	text.value = "COMBO *" + std::to_string(comboValue);

	vec2 position = {windowSize.x / 2 - 130.0f, windowSize.y - 350.0f};

	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = position;
	fg.scale = {2.0f, 2.0f};

	registry.colours.insert(entity, {1.0f, 1.0f, 1.0f, 1.0f});

	SlideUp& slideUp = registry.slideUps.emplace(entity);
	slideUp.fadeIn = true;
	slideUp.screenStartY = position.y;
	slideUp.animationLength = 2000.f;

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createScoreText(vec2 windowSize) {
	auto entity = Entity();

	registry.texts.emplace(entity);
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {(windowSize.x / 2) - 300.0f, windowSize.y - 70.0f};
	fg.scale = {1.5f, 1.5f};

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

float getElevation(vec2 xy)
{
	return 0.0f;
}
