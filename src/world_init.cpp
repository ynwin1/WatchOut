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

	// TODO LATER - A1 has this entity inserted into renderRequest container
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

	// TODO LATER - A1 has this entity inserted into renderRequest container
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

	// TODO LATER - A1 has this entity inserted into renderRequest container
	return entity;
};

// Trap creation
Entity createTrap(RenderSystem* renderer, vec2 pos)
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
	return entity;
};