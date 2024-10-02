#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hardcoded to the dimensions of the entity texture
// BB = bounding box
const float JEFF_BB_WIDTH   = 0.6f * 300.f;	// 1001
const float JEFF_BB_HEIGHT  = 0.6f * 202.f;	// 870

// the enemy
Entity createJeff(RenderSystem* renderer, vec2 position);
