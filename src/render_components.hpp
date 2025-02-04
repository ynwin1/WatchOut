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
	JEFF_PHANTOM_TRAP = JEFF_JUMP + 1,
	HEART = JEFF_PHANTOM_TRAP + 1,
	HEART_FADE = HEART + 1,
	TRAPCOLLECTABLE = HEART_FADE + 1,
	TRAPCOLLECTABLE_FADE = TRAPCOLLECTABLE + 1,
	TRAP = TRAPCOLLECTABLE_FADE + 1,
	BOW = TRAP + 1,
	BOW_FADE = BOW + 1,
	BOW_DRAW = BOW_FADE + 1,
	BOW_DRAWN = BOW_DRAW + 1,
	PHANTOM_TRAP_BOTTLE = BOW_DRAWN + 1,
	PHANTOM_TRAP_BOTTLE_FADE = PHANTOM_TRAP_BOTTLE + 1,
	PHANTOM_TRAP_BOTTLE_ONE = PHANTOM_TRAP_BOTTLE_FADE + 1,
	GRASS_TILE = PHANTOM_TRAP_BOTTLE_ONE + 1,
	TREE = GRASS_TILE + 1,
	SHRUB = TREE + 1,
	ROCK = SHRUB + 1,
	CLIFF = ROCK + 1,
	CLIFFSIDE = CLIFF + 1,
	CLIFFTOP = CLIFFSIDE + 1,
	MENU_HELP = CLIFFTOP + 1,
	MENU_PAUSED = MENU_HELP + 1,
	TUTORIAL_1 = MENU_PAUSED + 1,
	TUTORIAL_2 = TUTORIAL_1 + 1,
	TUTORIAL_3 = TUTORIAL_2 + 1,
	TUTORIAL_4 = TUTORIAL_3 + 1,
	BOAR_INTRO = TUTORIAL_4 + 1,
	BIRD_INTRO = BOAR_INTRO + 1,
	WIZARD_INTRO = BIRD_INTRO + 1,
	TROLL_INTRO = WIZARD_INTRO + 1,
	ARCHER_INTRO = TROLL_INTRO + 1,
	BARBARIAN_INTRO = ARCHER_INTRO + 1,
	BOMBER_INTRO = BARBARIAN_INTRO + 1,
	TUTORIAL_TARGET = BOMBER_INTRO + 1,
	HEART_INTRO = TUTORIAL_TARGET + 1,
	TRAP_INTRO = HEART_INTRO + 1,
	PHANTOM_TRAP_INTRO = TRAP_INTRO + 1,
	BOW_INTRO = PHANTOM_TRAP_INTRO + 1,
	BOMB_INTRO = BOW_INTRO + 1,
	BIRD_FLY = BOMB_INTRO + 1,
	BIRD_SWOOP = BIRD_FLY + 1,
	BIRD_DEAD = BIRD_SWOOP + 1,
	TROLL_RUN = BIRD_DEAD + 1,
	TROLL_DEAD = TROLL_RUN + 1,
	CROSSHAIR = TROLL_DEAD + 1,
	BOMBER_IDLE = CROSSHAIR + 1,
	BOMBER_RUN = BOMBER_IDLE + 1,
	BOMBER_DEAD = BOMBER_RUN + 1,
	BOMB = BOMBER_DEAD + 1,
	BOMB_FUSED = BOMB + 1,
	BOMB_FADE = BOMB_FUSED + 1,
	EXPLOSION = BOMB_FADE + 1,
	TITLE_BACKGROUND = EXPLOSION + 1,
	TITLE_TEXT = TITLE_BACKGROUND + 1,
	SMOKE = TITLE_TEXT + 1,
	TEXTURE_COUNT = SMOKE + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;


enum class EFFECT_ASSET_ID
{
	TEXTURED = 0,
	TEXTURED_FLAT = TEXTURED + 1,
	TEXTURED_NORMAL = TEXTURED_FLAT + 1,
	UNTEXTURED = TEXTURED_NORMAL + 1,
	ANIMATED = UNTEXTURED + 1,
	ANIMATED_NORMAL = ANIMATED + 1,
	FONT = ANIMATED_NORMAL + 1,
	TREE = FONT + 1,
	PARTICLE = TREE + 1,
	EFFECT_COUNT = PARTICLE + 1,
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

enum class PARTICLE {
	SMOKE,
	DASH
};

struct Particle
{
	PARTICLE type = PARTICLE::SMOKE;
	vec3 position = { 0, 0, 0 };
	float angle = 0;
	vec3 velocity = { 0, 0, 0 };
	vec2 scale = { 10, 10 };
	float gravity = 1.0;			// 1 means affected by gravity normally, 0 is no gravity
	float life = 10000.f;
};