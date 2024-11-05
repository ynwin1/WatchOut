#include "animation_system_init.hpp"
#include "tiny_ecs_registry.hpp"

const int JEFF_RUN_NUM_FRAMES = 6;
const int JEFF_RUN_FRAME_TIME = 150;
const int JEFF_IDLE_NUM_FRAMES = 4;
const int JEFF_IDLE_FRAME_TIME = 150;
const int JEFF_JUMP_NUM_FRAMES = 1;
const int JEFF_JUMP_FRAME_TIME = 1000;

const int BARBARIAN_RUN_NUM_FRAMES = 6;
const int BARBARIAN_RUN_FRAME_TIME = 150;
const int BARBARIAN_IDLE_NUM_FRAMES = 4;
const int BARBARIAN_IDLE_FRAME_TIME = 200;
const int BARBARIAN_DEAD_NUM_FRAMES = 1;
const int BARBARIAN_DEAD_FRAME_TIME = 1000;

const int BOAR_RUN_NUM_FRAMES = 7;
const int BOAR_RUN_FRAME_TIME = 80;
const int BOAR_IDLE_NUM_FRAMES = 1;
const int BOAR_IDLE_FRAME_TIME = 200;

const int ARCHER_RUN_NUM_FRAMES = 6;
const int ARCHER_RUN_FRAME_TIME = 150;
const int ARCHER_IDLE_NUM_FRAMES = 4;
const int ARCHER_IDLE_FRAME_TIME = 200;
const int ARCHER_DEAD_NUM_FRAMES = 1;
const int ARCHER_DEAD_FRAME_TIME = 1000;
const int ARCHER_BOWDRAW_NUM_FRAMES = 10;
const int ARCHER_BOWDRAW_FRAME_TIME = 100;

AnimationController& initJeffAnimationController(Entity& jeff) {
    AnimationController& animationcontroller = registry.animationControllers.emplace(jeff);
	animationcontroller.addAnimation(AnimationState::Idle, JEFF_IDLE_FRAME_TIME, JEFF_IDLE_NUM_FRAMES, TEXTURE_ASSET_ID::JEFF_IDLE);
    animationcontroller.addAnimation(AnimationState::Running, JEFF_RUN_FRAME_TIME, JEFF_RUN_NUM_FRAMES, TEXTURE_ASSET_ID::JEFF_RUN);
	animationcontroller.addAnimation(AnimationState::Jumping, JEFF_JUMP_FRAME_TIME, JEFF_JUMP_NUM_FRAMES, TEXTURE_ASSET_ID::JEFF_JUMP);

    registry.renderRequests.insert(
		jeff,
		{
			TEXTURE_ASSET_ID::JEFF_IDLE,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

    return animationcontroller;
}


AnimationController& initBarbarianAnimationController(Entity& entity) {
    AnimationController& animationcontroller = registry.animationControllers.emplace(entity);
	animationcontroller.addAnimation(AnimationState::Idle, BARBARIAN_IDLE_FRAME_TIME, BARBARIAN_IDLE_NUM_FRAMES, TEXTURE_ASSET_ID::BARBARIAN_IDLE);
    animationcontroller.addAnimation(AnimationState::Running, BARBARIAN_RUN_FRAME_TIME, BARBARIAN_RUN_NUM_FRAMES, TEXTURE_ASSET_ID::BARBARIAN_RUN);
	animationcontroller.addAnimation(AnimationState::Dead, BARBARIAN_DEAD_FRAME_TIME, BARBARIAN_DEAD_NUM_FRAMES, TEXTURE_ASSET_ID::BARBARIAN_DEAD);

    registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BARBARIAN_RUN,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	animationcontroller.changeState(entity, AnimationState::Running);

    return animationcontroller;
}

AnimationController& initBoarAnimationController(Entity& entity) {
    AnimationController& animationcontroller = registry.animationControllers.emplace(entity);
	animationcontroller.addAnimation(AnimationState::Idle, BOAR_IDLE_FRAME_TIME, BOAR_IDLE_NUM_FRAMES, TEXTURE_ASSET_ID::BOAR_IDLE);
    animationcontroller.addAnimation(AnimationState::Running, BOAR_RUN_FRAME_TIME, BOAR_RUN_NUM_FRAMES, TEXTURE_ASSET_ID::BOAR_RUN);
	animationcontroller.addAnimation(AnimationState::Dead, BOAR_IDLE_FRAME_TIME, BOAR_IDLE_NUM_FRAMES, TEXTURE_ASSET_ID::BOAR_IDLE);

    registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BOAR_RUN,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	animationcontroller.changeState(entity, AnimationState::Running);

    return animationcontroller;
}

AnimationController& initArcherAnimationController(Entity& entity) {
    AnimationController& animationcontroller = registry.animationControllers.emplace(entity);
	animationcontroller.addAnimation(AnimationState::Idle, ARCHER_IDLE_FRAME_TIME, ARCHER_IDLE_NUM_FRAMES, TEXTURE_ASSET_ID::ARCHER_IDLE);
    animationcontroller.addAnimation(AnimationState::Running, ARCHER_RUN_FRAME_TIME, ARCHER_RUN_NUM_FRAMES, TEXTURE_ASSET_ID::ARCHER_RUN);
	animationcontroller.addAnimation(AnimationState::Dead, ARCHER_DEAD_FRAME_TIME, ARCHER_DEAD_NUM_FRAMES, TEXTURE_ASSET_ID::ARCHER_DEAD);
	animationcontroller.addAnimation(AnimationState::Attack, ARCHER_BOWDRAW_FRAME_TIME, ARCHER_BOWDRAW_NUM_FRAMES, TEXTURE_ASSET_ID::ARCHER_BOW_DRAW);

    registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::ARCHER_RUN,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	animationcontroller.changeState(entity, AnimationState::Running);

    return animationcontroller;
}
