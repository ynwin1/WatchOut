#include "animation_system_init.hpp"
#include "tiny_ecs_registry.hpp"

const int JEFF_RUN_NUM_FRAMES = 6;
const int JEFF_RUN_FRAME_TIME = 150;

const int JEFF_IDLE_NUM_FRAMES = 4;
const int JEFF_IDLE_FRAME_TIME = 150;

AnimationController& initJeffAnimationController(Entity& jeff) {
    AnimationController& animationcontroller = registry.animationsControllers.emplace(jeff);
	animationcontroller.addAnimation(AnimationState::Idle, JEFF_IDLE_FRAME_TIME, JEFF_IDLE_NUM_FRAMES, TEXTURE_ASSET_ID::JEFF_IDLE);
    animationcontroller.addAnimation(AnimationState::Running, JEFF_RUN_FRAME_TIME, JEFF_RUN_NUM_FRAMES, TEXTURE_ASSET_ID::JEFF_RUN);

    registry.renderRequests.insert(
		jeff,
		{
			TEXTURE_ASSET_ID::JEFF_IDLE,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

    return animationcontroller;
}