#include "animation_system_init.hpp"
#include "tiny_ecs_registry.hpp"

const int JEFF_RUN_NUM_FRAMES = 6;
const int JEFF_RUN_FRAME_TIME = 150;
const TEXTURE_ASSET_ID JEFF_RUN_TEXTURE_ASSET_ID = TEXTURE_ASSET_ID::JEFF_RUN;

AnimationController& initJeffAnimationController(Entity& jeff) {
    printf("HERE0\n");
    AnimationController& animationcontroller = registry.animationsControllers.emplace(jeff);
    printf("HERE2");
    animationcontroller.addAnimation(AnimationState::Idle, JEFF_RUN_FRAME_TIME, JEFF_RUN_NUM_FRAMES);

    registry.renderRequests.insert(
		jeff,
		{
			TEXTURE_ASSET_ID::JEFF_RUN,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

    return animationcontroller;
}