#pragma once

struct Animation
{
	int currentFrame = 0;
	float elapsedTime = 0.0f;
	float frameTime;
	int numFrames;

	Animation(float frameTime, int numFrames)
		: currentFrame(0), elapsedTime(0.0f), frameTime(frameTime), numFrames(numFrames) {}
};

void updateAnimation(Animation& animation, float deltaTime);