#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hardcoded to the dimensions of the entity texture
// BB = bounding box
const float JEFF_BB_WIDTH   = 20.f * 10.f;
const float JEFF_BB_HEIGHT  = 28.f * 10.f;

// the enemy
Entity createJeff(RenderSystem* renderer, vec2 position);

Entity createBarbarian(RenderSystem* renderer, vec2 position);
