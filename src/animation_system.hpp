#include <unordered_map>
#include "render_components.hpp"
#pragma once

enum class AnimationState { Idle, Running, Jumping };

// Represents a single animation sequence, including frame timing, frame count, and spritesheet
// Gets looked up if EFFECT_ASSET_ID is ANIMATED
struct Animation
{
	int currentFrame = 0;
	float elapsedTime = 0.0f;
	float frameTime = 0.;
	int numFrames = 1;
	TEXTURE_ASSET_ID spritesheet;


	Animation() = default;

	Animation(float frameTime, int numFrames, TEXTURE_ASSET_ID spritesheet)
		: currentFrame(0), elapsedTime(0.0f), frameTime(frameTime), numFrames(numFrames), spritesheet(spritesheet){}
};

// Controls and manages animations for an entity by storing animations mapped to states
struct AnimationController	
{
	std::unordered_map<AnimationState, Animation> animations;
	AnimationState currentState;

	AnimationController() : currentState(AnimationState::Idle) {}

	void addAnimation(AnimationState state, float frameTime, int numFrames, TEXTURE_ASSET_ID spritesheet)
	{
		animations[state] = Animation(frameTime, numFrames, spritesheet);
	}
    
	void changeState(Entity entity, AnimationState newState);
};

void updateAnimation(Animation& animation, float deltaTime);