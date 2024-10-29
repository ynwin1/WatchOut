#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Boar creation
Entity createBoar(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + BOAR_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT, BOAR_BB_HEIGHT };

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 20;
	enemy.cooldown = 1500.f; // 1.5s

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
Entity createBarbarian(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + BARBARIAN_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT };
	
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 30;
	enemy.cooldown = 2000.f; // 2s

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
Entity createArcher(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + ARCHER_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { ARCHER_BB_WIDTH, ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT };

	// Add Render Request for drawing sprite
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = 40;
	enemy.cooldown = 3000.f; // 3s

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
Entity createCollectibleTrap(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();
	registry.collectibleTraps.emplace(entity);

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TRAP_COLLECTABLE_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT };

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
Entity createHeart(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();
	registry.hearts.emplace(entity);

	// Setting intial motion values
	Motion& fixed = registry.motions.emplace(entity);
	fixed.position = vec3(pos, getElevation(pos) + HEART_BB_WIDTH / 2);
	fixed.angle = 0.f;
	fixed.scale = { HEART_BB_WIDTH, HEART_BB_WIDTH };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { HEART_BB_WIDTH, HEART_BB_WIDTH, HEART_BB_WIDTH };

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
Entity createDamageTrap(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TRAP_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { TRAP_BB_WIDTH, TRAP_BB_WIDTH, TRAP_BB_HEIGHT };

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
Entity createJeff(RenderSystem* renderer, vec2 position)
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

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ JEFF_BB_WIDTH, JEFF_BB_HEIGHT });

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.dimension = { JEFF_BB_WIDTH, JEFF_BB_WIDTH, JEFF_BB_HEIGHT };

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

Entity createSalmon(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, 0);
	motion.angle = 0.f;
	motion.scale = mesh.original_size * 150.f;
	motion.scale.x *= -1; // point front to the right
	motion.scale.y *= -1;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no texture is needed
			EFFECT_ASSET_ID::SALMON,
			GEOMETRY_BUFFER_ID::SALMON });
	registry.midgrounds.emplace(entity);

	return entity;
}

void createBattleGround() {
	auto entity = Entity();

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BATTLEGROUND,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::GAME_SPACE
		});
	registry.backgrounds.emplace(entity);
}

// Gameover
Entity createGameOver(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, 0);
	motion.angle = 0.f;
	motion.scale = { GO_BB_WIDTH, GO_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);


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

float getElevation(vec2 xy)
{
	return 0.0f;
}
