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
	BARBARIAN_IDLE = NONE + 1,
	BARBARIAN_RUN = BARBARIAN_IDLE + 1,
	BARBARIAN_DEAD = BARBARIAN_RUN + 1,
	BOAR_IDLE = BARBARIAN_DEAD + 1,
	BOAR_RUN = BOAR_IDLE + 1,
	ARCHER_IDLE = BOAR_RUN + 1,
	ARCHER_RUN = ARCHER_IDLE + 1,
	ARCHER_DEAD = ARCHER_RUN + 1,
	ARCHER_BOW_DRAW = ARCHER_DEAD + 1,
	ARROW = ARCHER_BOW_DRAW + 1,
	WIZARD_IDLE = ARROW + 1,
	WIZARD_RUN = WIZARD_IDLE + 1,
	WIZARD_DEAD = WIZARD_RUN + 1,
	FIREBALL = WIZARD_DEAD + 1,
	LIGHTNING = FIREBALL + 1,
	TARGET_AREA = LIGHTNING + 1,
	JEFF_RUN = TARGET_AREA + 1,
	JEFF_IDLE = JEFF_RUN + 1,
	JEFF_JUMP = JEFF_IDLE + 1,
	HEART = JEFF_JUMP + 1,
	HEART_FADE = HEART + 1,
	TRAPCOLLECTABLE = HEART_FADE + 1,
	TRAPCOLLECTABLE_FADE = TRAPCOLLECTABLE + 1,
	TRAP = TRAPCOLLECTABLE_FADE + 1,
	GRASS_TILE = TRAP + 1,
	TREE = GRASS_TILE + 1,
	SHRUB = TREE + 1,
	ROCK = SHRUB + 1,
	CLIFF = ROCK + 1,
	CLIFFSIDE = CLIFF + 1,
	CLIFFTOP = CLIFFSIDE + 1,
	MENU_HELP = CLIFFTOP + 1,
	MENU_PAUSED = MENU_HELP + 1,
	BIRD_FLY = MENU_PAUSED + 1,
	BIRD_SWOOP = BIRD_FLY + 1,
	BIRD_DEAD = BIRD_SWOOP + 1,
	TROLL_RUN = BIRD_DEAD + 1,
	TROLL_DEAD = TROLL_RUN + 1,
	TEXTURE_COUNT = TROLL_DEAD + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class NORMAL_ASSET_ID
{
	NONE = -1,
	ROCK = NONE + 1,
	NORMAL_COUNT = ROCK + 1
};
const int normal_count = (int)NORMAL_ASSET_ID::NORMAL_COUNT;

enum class EFFECT_ASSET_ID
{
	TEXTURED = 0,
	TEXTURED_FLAT = TEXTURED + 1,
	TEXTURED_NORMAL = TEXTURED_FLAT + 1,
	UNTEXTURED = TEXTURED_NORMAL + 1,
	ANIMATED = UNTEXTURED + 1,
	FONT = ANIMATED + 1,
	TREE = FONT + 1,
	EFFECT_COUNT = TREE + 1,
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
	SPRITE = 0,
	GAME_SPACE = SPRITE + 1,
	RECTANGLE = GAME_SPACE + 1,
	TEXT = RECTANGLE + 1,
	TREE = TEXT + 1,
	MAP_TILE = TREE + 1,
	OBSTACLE = MAP_TILE + 1,
	GEOMETRY_COUNT = OBSTACLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class PRIMITIVE_TYPE {
	TRIANGLES,
	LINES
};

struct RenderRequest
{
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
	PRIMITIVE_TYPE primitive_type = PRIMITIVE_TYPE::TRIANGLES;
	NORMAL_ASSET_ID used_normal = NORMAL_ASSET_ID::NONE;
};

struct Background {};
struct Midground {};
struct Foreground {
	vec2 position;
	vec2 scale;
};

struct PointLight {
	vec3 position;

	vec4 ambient;
	vec4 diffuse;

	float max_distance;

	float constant;
	float linear;
	float quadratic;
};