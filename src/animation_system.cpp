#include "animation_system.hpp"

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