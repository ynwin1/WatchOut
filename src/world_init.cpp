#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"


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

Entity createBarbarian(RenderSystem* renderer, vec2 position)
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
			TEXTURE_ASSET_ID::BARBARIAN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}