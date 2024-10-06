#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Player creation
Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT };

	// Setting initial hitbox values
	Hitbox& hitbox = registry.hitboxes.emplace(entity);
	hitbox.position = pos;
	hitbox.dimension = { PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT };

	// Setting initial player values
	registry.players.emplace(entity);
	// TODO LATER - A1 has this entity inserted into renderRequest container
	return entity;
};

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
	enemy.damage = 3;

	// Add Render Request for drawing sprite
	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::BOAR,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
	
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
	enemy.damage = 5;

	// Add Render Request for drawing sprite
	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::BARBARIAN,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
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
	enemy.damage = 5;

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::ARCHER,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});
	
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

Entity createJeff(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ JEFF_BB_WIDTH, JEFF_BB_HEIGHT });

	// create an empty Eel component to be able to refer to all eels
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::JEFF,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}