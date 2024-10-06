#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// hardcoded dimensions of player and enemies (boar, babarian, and archer)
// BB = Bounding Box
const float SPRITE_SCALE = 3.f;
const float PLAYER_BB_WIDTH = 0.5f;
const float PLAYER_BB_HEIGHT = 0.5f;
const float BOAR_BB_WIDTH = 32.f * SPRITE_SCALE;
const float BOAR_BB_HEIGHT = 32.f * SPRITE_SCALE;
const float BARBARIAN_BB_WIDTH = 20.f * SPRITE_SCALE;
const float BARBARIAN_BB_HEIGHT = 34.f * SPRITE_SCALE;
const float ARCHER_BB_WIDTH = 32.f * SPRITE_SCALE;
const float ARCHER_BB_HEIGHT = 32.f * SPRITE_SCALE;
const float TRAP_BB_WIDTH = 0.3f;
const float TRAP_BB_HEIGHT = 0.2f;
const float JEFF_BB_WIDTH   = 20.f * SPRITE_SCALE;
const float JEFF_BB_HEIGHT  = 28.f * SPRITE_SCALE;
const float JEFF_ANIMATED_BB_WIDTH  = 128.f * SPRITE_SCALE;
const float JEFF_ANIMATED_BB_HEIGHT  = 32.f * SPRITE_SCALE;

// The player
Entity createPlayer(RenderSystem* renderer, vec2 pos);

// The boar
Entity createBoar(RenderSystem* renderer, vec2 pos);

// The barbarian
Entity createBarbarian(RenderSystem* renderer, vec2 pos);

// The archer
Entity createArcher(RenderSystem* renderer, vec2 pos);

// The collectible trap
Entity createCollectibleTrap(RenderSystem* renderer, vec2 pos);

// The damage trap
Entity createDamageTrap(RenderSystem* renderer, vec2 pos);

// Jeff the Player
Entity createJeff(RenderSystem* renderer, vec2 position);

// Animated Jeff
Entity createAnimatedJeff(RenderSystem* renderer, vec2 position);