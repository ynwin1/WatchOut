#include "animation_system.hpp"
#include "tiny_ecs_registry.hpp"

// Update frame based on time in ms
void updateAnimation(Animation& animation, float deltaTime)
{
	animation.elapsedTime += deltaTime;
	if (animation.elapsedTime >= animation.frameTime)
	{
		animation.currentFrame = (animation.currentFrame + 1) % animation.numFrames;
		animation.elapsedTime = 0.0f;
	}
}

void AnimationController::changeState(Entity entity, AnimationState newState)
{
	if (currentState != newState)
	{
		currentState = newState;
		animations[currentState].currentFrame = 0; 
		animations[currentState].elapsedTime = 0.0f;

		// Update used texture in render request
		Animation& newAnimation = animations.at(newState);
		registry.renderRequests.get(entity).used_texture = newAnimation.spritesheet;
	}
}