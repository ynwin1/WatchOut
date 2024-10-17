#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Boar creation
Entity createBoar(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = pos;
	hitbox.dimension = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT };

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

	createHealthBar(entity, vec3(1.0f, 0.0f, 0.0f));
	
	return entity;
};

// Barbarian creation
Entity createBarbarian(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = pos;
	hitbox.dimension = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT };
	
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

	createHealthBar(entity, vec3(1.0f, 0.0f, 0.0f));

	return entity;
};

// Archer creation
Entity createArcher(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = pos;
	hitbox.dimension = { ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT };

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

	createHealthBar(entity, vec3(1.0f, 0.0f, 0.0f));
	
	return entity;
};

// Collectible trap creation
Entity createCollectibleTrap(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = pos;
	hitbox.dimension = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };

	// TODO LATER - A1 has this entity inserted into renderRequest container
	registry.collectibles.emplace(entity);
	return entity;
};

// Damage trap creation
Entity createDamageTrap(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = pos;
	hitbox.dimension = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };

	// Setting initial trap values
	registry.traps.emplace(entity);
	// TODO LATER - A1 has this entity inserted into renderRequest container
	return entity;
};

// Create Player Jeff
Entity createJeff(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0.f };
	motion.position = position;

	//Initialize movement
	auto& player = registry.players.emplace(entity);
	player.isJumping = false;
	player.isRolling = false;
	player.isRunning = false;
	player.facing = { 1, 0 };

	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ JEFF_BB_WIDTH, JEFF_BB_HEIGHT });

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = position;
	hitbox.dimension = { JEFF_BB_WIDTH, JEFF_BB_HEIGHT };

	// Jeff Render Request
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::JEFF,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	createHealthBar(entity, vec3(0.0f, 1.0f, 0.0f));
	
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
}

// gameover
Entity createGameOver(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
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
	
	return entity;
};

void createHealthBar(Entity characterEntity, vec3 color) {
	auto meshEntity = Entity();

	const float width = 60.0f;
	const float height = 10.0f;

	Motion& characterMotion = registry.motions.get(characterEntity);

	StaticMotion& staticMotion = registry.staticMotions.emplace(meshEntity);
	staticMotion.position = characterMotion.position;
	staticMotion.angle = 0.f;
	staticMotion.scale = { width, height };

	registry.colours.insert(meshEntity, color);

	registry.renderRequests.insert(
		meshEntity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::HEALTH_BAR
		});

	HealthBar& hpbar = registry.healthBars.emplace(characterEntity, meshEntity);
	hpbar.width = width;
	hpbar.height = height;
}

void createFPSText(vec2 position) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00";
	text.position = position;

	registry.renderRequests.insert(
			entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::TEXT,
			GEOMETRY_BUFFER_ID::TEXT
		});
}