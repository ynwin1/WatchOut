#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <iostream>

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

struct UntexturedVertex
{
	vec3 position;
};

struct Animation
{
	int currentFrame = 0;
	float elapsedTime = 0.0f;
	float frameTime;
	int numFrames;

	Animation(float frameTime, int numFrames)
		: currentFrame(0), elapsedTime(0.0f), frameTime(frameTime), numFrames(numFrames) {}

	// Update frame based on time in ms
	void update(float deltaTime)
	{
		elapsedTime += deltaTime;
		if (elapsedTime >= frameTime)
		{
			currentFrame = (currentFrame + 1) % numFrames;
			elapsedTime = 0.0f;
		}
	}
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
	NONE = -1,
	JEFF = 0,
	BARBARIAN = JEFF + 1,
	BOAR = BARBARIAN + 1,
	ARCHER = BOAR + 1,
	BATTLEGROUND = ARCHER + 1,
	GAMEOVER = BATTLEGROUND + 1,
	JEFF_RUN = GAMEOVER + 1,
	TEXTURE_COUNT = JEFF_RUN + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	TEXTURED = 0,
	UNTEXTURED = 1,
	ANIMATED = 2,
	EFFECT_COUNT = ANIMATED + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
	SPRITE = 0,
	GAME_SPACE = SPRITE + 1,
	HEALTH_BAR = GAME_SPACE + 1,
	GEOMETRY_COUNT = HEALTH_BAR + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};