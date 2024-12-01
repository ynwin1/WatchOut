#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "animation_system.hpp"
#include "animation_system_init.hpp"
#include "ai_system.hpp"
#include <random>
#include <sstream>

// Boar creation
Entity createBoar(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + BOAR_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT };
	motion.hitbox = { BOAR_BB_WIDTH, BOAR_BB_HEIGHT, BOAR_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = BOAR_DAMAGE;
	enemy.maxHealth = BOAR_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = BOAR_SPEED;

	registry.boars.emplace(entity);
	
	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	initBoarAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	registry.knockers.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = BOAR_SPEED;
	
	return entity;
};

// Barbarian creation
Entity createBarbarian(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + BARBARIAN_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { 32. * SPRITE_SCALE, 36. * SPRITE_SCALE};
	motion.hitbox = { BARBARIAN_BB_WIDTH, BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT / zConversionFactor };
	motion.solid = true;
	
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = BARBARIAN_DAMAGE;
	enemy.cooldown = 1000;
	enemy.maxHealth = BARBARIAN_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = BARBARIAN_SPEED;

	registry.barbarians.emplace(entity);

	initBarbarianAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	registry.knockers.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = BARBARIAN_SPEED;

	return entity;
};

// Archer creation
Entity createArcher(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + ARCHER_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT };
	motion.hitbox = { ARCHER_BB_WIDTH, ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = ARCHER_DAMAGE;
	enemy.maxHealth = ARCHER_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = ARCHER_SPEED;

	registry.archers.emplace(entity);

	initArcherAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = ARCHER_SPEED;
	
	return entity;
};

// Bird creation (Flock of 5 birds)
Entity createBirdFlock(vec2 pos)
{
    const int flockSize = 5;
    const float spacing = 20.f; 
    Entity repBird;

    for (int i = 0; i < flockSize; ++i)
    {
		// Spawn birds with spacing
		vec2 birdPosition = pos + vec2(i % 2, (i % 2 + 1)) * spacing;
		Entity bird = createBird(birdPosition);
		if (i == 0) {
			repBird = bird;
		}
    }
    return repBird;
}

Entity createBird(vec2 birdPosition) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(birdPosition, TREE_BB_HEIGHT - BIRD_BB_WIDTH);
	motion.angle = 0.f;
	motion.scale = { 16 * SPRITE_SCALE, 16 * SPRITE_SCALE };
	motion.hitbox = { BIRD_BB_WIDTH, BIRD_BB_HEIGHT, BIRD_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = BIRD_DAMAGE;
	enemy.cooldown = 2000.f;
	enemy.maxHealth = BIRD_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = BIRD_SPEED;

	registry.birds.emplace(entity);

	initBirdAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));
	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = BIRD_SPEED;

	return entity;
}
// Wizard creation
Entity createWizard(vec2 pos) {
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + WIZARD_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { 96 * SPRITE_SCALE,  35 * SPRITE_SCALE };
	motion.hitbox = { WIZARD_BB_WIDTH, WIZARD_BB_WIDTH, WIZARD_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = WIZARD_DAMAGE;
	enemy.cooldown = 8000.f; // 8s
	enemy.maxHealth = WIZARD_HEALTH;
	enemy.health = enemy.maxHealth;
	motion.speed = WIZARD_SPEED;

	registry.wizards.emplace(entity);

	initWizardAnimationController(entity);
	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = WIZARD_SPEED;

	return entity;
}

Entity createTroll(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TROLL_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TROLL_BB_WIDTH, TROLL_BB_HEIGHT };
	motion.hitbox = { TROLL_BB_WIDTH * 0.9, TROLL_BB_WIDTH * 0.9, TROLL_BB_HEIGHT * 0.9 / zConversionFactor };
	motion.solid = true;
	if (registry.players.entities.size() > 0) {
		vec2 playerPosition = vec2(registry.motions.get(registry.players.entities.at(0)).position);
		motion.facing = normalize(playerPosition - pos);
	}

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.damage = TROLL_DAMAGE;
	enemy.cooldown = 0;
	motion.speed = TROLL_SPEED;
	enemy.maxHealth = TROLL_HEALTH;
	enemy.health = enemy.maxHealth;

	registry.trolls.emplace(entity);

	registry.midgrounds.emplace(entity);

	createHealthBar(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = TROLL_SPEED;
	Knocker& knocker = registry.knockers.emplace(entity);
	knocker.strength = 1.5f;

	initTrollAnimationController(entity);

	return entity;
}
;

// Collectible trap creation
Entity createCollectibleTrap(vec2 pos)
{
	auto entity = Entity();
	CollectibleTrap& collectibleTrap = registry.collectibleTraps.emplace(entity);
	int random = rand() % 2;
	Motion& motion = registry.motions.emplace(entity);

	if (random >= 0.5) {
		collectibleTrap.type = "phantom_trap";
		initPhantomTrapAnimationController(entity);
		
		motion.position = vec3(pos, getElevation(pos) + PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT / 2);
		motion.angle = 0.f;
		motion.scale = { PHANTOM_TRAP_COLLECTABLE_BB_WIDTH, PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT };
		motion.hitbox = { PHANTOM_TRAP_COLLECTABLE_BB_WIDTH, PHANTOM_TRAP_COLLECTABLE_BB_WIDTH, PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT / zConversionFactor };
	}
	else {
		initTrapBottleAnimationController(entity);

		motion.position = vec3(pos, getElevation(pos) + TRAP_COLLECTABLE_BB_HEIGHT / 2);
		motion.angle = 0.f;
		motion.scale = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT };
		motion.hitbox = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT / zConversionFactor };
	}

	registry.collectibles.emplace(entity);

	registry.midgrounds.emplace(entity);

	printf("Collectible trap created\n");

	return entity;
};

Entity createCollectible(vec2 pos, TEXTURE_ASSET_ID assetID)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;

	Collectible& collecitble = registry.collectibles.emplace(entity);

	switch(assetID) {
		case TEXTURE_ASSET_ID::HEART:
			motion.scale = { HEART_BB_WIDTH, HEART_BB_WIDTH };
			initHeartAnimationController(entity);
			break;
		case TEXTURE_ASSET_ID::TRAP:
			motion.scale = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT };
			initTrapBottleAnimationController(entity);
			break;
		case TEXTURE_ASSET_ID::BOW:
			motion.scale = { BOW_BB_WIDTH, BOW_BB_HEIGHT };
			registry.bows.emplace(entity);
			collecitble.duration = 10000;
			initBowAnimationController(entity);
			break;
		default:
			break;
	}

	motion.position = vec3(pos, getElevation(pos) + motion.scale.y / 2);
	motion.hitbox = { motion.scale.x, motion.scale.x, motion.scale.y / zConversionFactor };

	registry.midgrounds.emplace(entity);

	return entity;
};

// Heart creation
Entity createHeart(vec2 pos)
{
	auto entity = Entity();
	registry.hearts.emplace(entity);

	// Setting intial motion values
	Motion& fixed = registry.motions.emplace(entity);
	fixed.position = vec3(pos, getElevation(pos) + HEART_BB_WIDTH / 2);
	fixed.angle = 0.f;
	fixed.scale = { HEART_BB_WIDTH, HEART_BB_WIDTH };
	fixed.hitbox = { HEART_BB_WIDTH, HEART_BB_WIDTH, HEART_BB_HEIGHT / zConversionFactor };

	registry.collectibles.emplace(entity);

	initHeartAnimationController(entity);

	registry.midgrounds.emplace(entity);

	return entity;
};

Entity createCollected(Motion& playerM, vec2 size, TEXTURE_ASSET_ID assetID)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = { size.x * 0.7, size.y * 0.7};

	registry.collected.emplace(entity);
	registry.midgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
};

// Damage trap creation
Entity createDamageTrap(vec2 pos)
{
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + TRAP_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { TRAP_BB_WIDTH, TRAP_BB_HEIGHT };
	motion.hitbox = { TRAP_BB_WIDTH, TRAP_BB_WIDTH, TRAP_BB_HEIGHT / zConversionFactor };

	// Setting initial trap values
	registry.traps.emplace(entity);

	registry.renderRequests.insert(
	entity,
	{
		TEXTURE_ASSET_ID::TRAP,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE
	});

	registry.backgrounds.emplace(entity);

	return entity;
};

Entity createPhantomTrap(vec2 pos) {
	auto entity = Entity();

	// Setting intial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, getElevation(pos) + PHANTOM_TRAP_BB_HEIGHT / 2);
	motion.angle = 0.f;
	motion.scale = { PHANTOM_TRAP_BB_WIDTH, PHANTOM_TRAP_BB_HEIGHT };
	motion.hitbox = { PHANTOM_TRAP_BB_WIDTH, PHANTOM_TRAP_BB_WIDTH, PHANTOM_TRAP_BB_HEIGHT / zConversionFactor };
	motion.solid = false;

	// Setting initial trap values
	PhantomTrap& phantomTrap = registry.phantomTraps.emplace(entity);
	phantomTrap.duration = 15000.f; // 7s

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::JEFF_PHANTOM_TRAP,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	registry.backgrounds.emplace(entity);

	return entity;
}


// Create Player Jeff
Entity createJeff(vec2 position)
{
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = vec3(position, getElevation(position) + JEFF_BB_HEIGHT / 2);
	motion.facing = { 1, 0 };

	//Initialize stamina
	auto& stamina = registry.staminas.emplace(entity);

	//Initialize movement
	auto& player = registry.players.emplace(entity);
	player.isRolling = false;
	player.isRunning = false;

	auto& dasher = registry.dashers.emplace(entity);
	dasher.isDashing = false;
	dasher.dashStartPosition = { 0, 0 };
	dasher.dashTargetPosition = { 0, 0 };
	dasher.dashTimer = 0.0f;
	dasher.dashDuration = 0.2f;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 32. * SPRITE_SCALE, 32. * SPRITE_SCALE});
	motion.hitbox = { JEFF_BB_WIDTH, JEFF_BB_WIDTH, JEFF_BB_HEIGHT / zConversionFactor };
	motion.solid = true;
	motion.speed = PLAYER_SPEED;

	auto& jumper = registry.jumpers.emplace(entity);
	jumper.speed = 2;

	// Animation
	initJeffAnimationController(entity);
	registry.midgrounds.emplace(entity);

	registry.knockables.emplace(entity);
	auto& trappable = registry.trappables.emplace(entity);
	trappable.originalSpeed = PLAYER_SPEED;
	

	auto& pointLight = registry.pointLights.emplace(entity);
	pointLight.position = motion.position;
	pointLight.ambient = vec4(1.0, .75, 0.25, .2);
	pointLight.diffuse = vec4(1.0, .75, 0.25, 1.0);
	pointLight.max_distance = 3250;
	pointLight.constant = 1.0;
	pointLight.linear = .00014;
	pointLight.quadratic = 0.00001;
	
	return entity;
}

Entity createTree(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TREE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(pos, 0);
	motion.angle = 0.f;
	motion.scale = { TREE_BB_WIDTH, TREE_BB_HEIGHT };
	motion.hitbox = { TREE_BB_WIDTH, TREE_BB_WIDTH, TREE_BB_HEIGHT / zConversionFactor };
	motion.solid = true;

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TREE,
			EFFECT_ASSET_ID::TEXTURED_NORMAL,
			GEOMETRY_BUFFER_ID::SPRITE });

	/*if (!RENDERING_MESH) {
		registry.renderRequests.insert(
			entity, {
				TEXTURE_ASSET_ID::TREE,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity, {
				TEXTURE_ASSET_ID::TREE,
				EFFECT_ASSET_ID::TREE,
				GEOMETRY_BUFFER_ID::TREE });
	}*/

	registry.obstacles.emplace(entity);
	registry.midgrounds.emplace(entity);

	return entity;
}

Entity createArrow(vec3 pos, vec3 velocity, int damage)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = velocity;
	motion.scale = { ARROW_BB_WIDTH, ARROW_BB_HEIGHT };
	motion.hitbox = { ARROW_BB_WIDTH, ARROW_BB_HEIGHT, ARROW_BB_HEIGHT / zConversionFactor };
	
	registry.projectiles.emplace(entity);
	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.damage = damage;
	registry.midgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createFireball(vec3 pos, vec2 direction) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = vec3(0);
	motion.angle = atan2(direction.y, direction.x);
	motion.scale = { FIREBALL_BB_WIDTH, FIREBALL_BB_HEIGHT };
	motion.hitbox = { FIREBALL_HITBOX_WIDTH, FIREBALL_HITBOX_WIDTH, FIREBALL_HITBOX_WIDTH };

	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.type = "fireball";
	damaging.damage = 30;
	registry.midgrounds.emplace(entity);

	initFireballAnimationController(entity);
	return entity;
}


Entity createEquipped(TEXTURE_ASSET_ID assetId) {
	auto entity = Entity();
	vec2 scale;

	switch (assetId) {
		case TEXTURE_ASSET_ID::TRAPCOLLECTABLE:
			scale = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT};
		break;
		case TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE:
			scale = { PHANTOM_TRAP_COLLECTABLE_BB_WIDTH * 0.8, PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT * 0.8};
		break;
		case TEXTURE_ASSET_ID::BOW:
			scale = { BOW_BB_WIDTH * 1.25, BOW_BB_HEIGHT * 1.25};
			AnimationController& ac = initBowAnimationController(entity);
			ac.changeState(entity, AnimationState::Default);
		break;
	}

	if(assetId != TEXTURE_ASSET_ID::BOW) {
		registry.renderRequests.insert(
			entity,
			{
				assetId,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
			});
	}

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = scale;

	registry.midgrounds.emplace(entity);

	return entity;
}

Entity createLightning(vec2 pos) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	
	// add half the hitbox size to the vec2 pos
	motion.scale = { LIGHTNING_BB_WIDTH, LIGHTNING_BB_HEIGHT };
	motion.hitbox = { LIGHTNING_BB_WIDTH, LIGHTNING_BB_WIDTH, LIGHTNING_BB_HEIGHT / zConversionFactor };
	motion.position = vec3(pos, motion.hitbox.z / 2);

	Damaging& damaging = registry.damagings.emplace(entity);
	damaging.type = "lightning";
	damaging.damage = 20;
	registry.midgrounds.emplace(entity);

	Cooldown& duration = registry.cooldowns.emplace(entity);
	duration.remaining = 1500.f;

	initLightningAnimationController(entity);
	return entity;
}

void createPlayerStaminaBar(Entity characterEntity, vec2 windowSize) {
	auto meshE = Entity();
	const float width = 150.0f;
	const float height = 20.0f;

	vec2 position = {210.0f, windowSize.y - 80.0f};

	Foreground& fg = registry.foregrounds.emplace(meshE);
	fg.position = position;
	fg.scale = { width, height };

	vec4 colour = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	registry.colours.insert(meshE, colour);

	registry.renderRequests.insert(
		meshE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE
		});

	// Bar frame
	auto frameE = Entity();
	Foreground& frameFg = registry.foregrounds.emplace(frameE);
	frameFg.position = position;
	frameFg.scale = { width, height };
	registry.colours.insert(frameE, colour);
	registry.renderRequests.insert(
		frameE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			PRIMITIVE_TYPE::LINES,
		});

	auto textE = Entity();
	registry.texts.emplace(textE);
	Foreground& textFg = registry.foregrounds.emplace(textE);
	textFg.scale = {0.8f, 0.8f};
	textFg.position = {position.x - 100.0f, position.y};
	registry.renderRequests.insert(
		textE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT,
		});

	StaminaBar& staminabar = registry.staminaBars.emplace(characterEntity, meshE, frameE);
	staminabar.width = width;
	staminabar.height = height;
	staminabar.textEntity = textE;
}

void createPlayerHealthBar(Entity characterEntity, vec2 windowSize) {
	auto meshE= Entity();
	const float width = 150.0f;
	const float height = 20.0f;

	vec2 position = {210.0f, windowSize.y - 50.0f};

	Foreground& fg = registry.foregrounds.emplace(meshE);
	fg.position = position;
	fg.scale = {width, height};

	vec4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	registry.colours.insert(meshE, green);

	registry.renderRequests.insert(
		meshE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
		});

	// Bar frame
	auto frameE = Entity();
	Foreground& frameFg = registry.foregrounds.emplace(frameE);
	frameFg.position = position;
	frameFg.scale = { width, height };
	registry.colours.insert(frameE, green);
	registry.renderRequests.insert(
		frameE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			PRIMITIVE_TYPE::LINES,
		});

	auto textE = Entity();
	registry.texts.emplace(textE);
	Foreground& textFg = registry.foregrounds.emplace(textE);
	textFg.scale = {0.8f, 0.8f};
	textFg.position = {position.x - 32.0f, position.y};
	registry.renderRequests.insert(
		textE,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT,
		});
	HealthBar& hpbar = registry.healthBars.emplace(characterEntity, meshE, frameE);
	hpbar.width = width;
	hpbar.height = height;
	hpbar.textEntity = textE;
}

void createHealthBar(Entity characterEntity, vec4 color) {
	auto meshEntity = Entity();

	const float width = 60.0f;
	const float height = 10.0f;

	Motion& characterMotion = registry.motions.get(characterEntity);

	Motion& motion = registry.motions.emplace(meshEntity);
	// position does not need to be initialized as it will always be set to match the associated entity
	motion.angle = 0.f;
	motion.scale = { width, height };

	registry.colours.insert(meshEntity, color);

	registry.renderRequests.insert(
		meshEntity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
		});
	registry.midgrounds.emplace(meshEntity);

	// HP bar frame
	auto frameEntity = Entity();
	Motion& frameM = registry.motions.emplace(frameEntity);
	frameM.position = motion.position;
	frameM.scale = { width, height };
	registry.colours.insert(frameEntity, color);
	registry.renderRequests.insert(
		frameEntity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			PRIMITIVE_TYPE::LINES,
		});
	registry.midgrounds.emplace(frameEntity);

	HealthBar& hpbar = registry.healthBars.emplace(characterEntity, meshEntity, frameEntity);
	hpbar.width = width;
	hpbar.height = height;
}

Entity createTargetArea(vec3 position) {
	auto entity = Entity();

	float radius = 200.f;
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.position.z = 0.f;

	float ogRadius = 170.f;
	float scaledFactor = radius / ogRadius;
	motion.scale = { 2 * ogRadius * scaledFactor, 2 * ogRadius * scaledFactor * zConversionFactor };

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TARGET_AREA,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	registry.backgrounds.emplace(entity);
	registry.targetAreas.emplace(entity);

	Cooldown& cooldown = registry.cooldowns.emplace(entity);
	cooldown.remaining = 3000.f; // 3s
	return entity;
}

Entity createPauseHelpText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "PAUSE/PLAY(P)    HELP (H)";
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {windowSize.x - 550, windowSize.y - 70.0f};
	fg.scale = {1.5f, 1.5f};

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createFPSText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00 fps";
	// text position based on screen coordinates
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {30.0f, windowSize.y - 40.0f};
	fg.scale = {0.8f, 0.8f};

	registry.renderRequests.insert(
			entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createGameTimerText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "00:00:00";
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {(windowSize.x / 2) + 50.0f, windowSize.y - 80.0f};
	fg.scale = {2.0f, 2.0f};

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	return entity;
}

Entity createItemCountText(vec2 windowSize, TEXTURE_ASSET_ID assetID) {
	auto textKeybindE = Entity();
	auto textCountE = Entity();
	auto iconE = Entity();
	vec2 startPos = {420.0f, windowSize.y - 30.0f};
	vec2 iconScale;
	vec2 position;
	vec2 keybindPos;
	vec2 iconPos;
	vec2 textPos;
	std::string keybind;

	switch(assetID) {
		case TEXTURE_ASSET_ID::TRAPCOLLECTABLE:
			iconScale = { TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT};
			position = startPos;
			keybindPos = position + vec2(-5.0f, 0.0f);
			iconPos = keybindPos + vec2(0.0f, -40.0f);
			textPos = iconPos + vec2(-30.0f, -55.0f);
			keybind = "1";
			break;
		case TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE:
			iconScale = { PHANTOM_TRAP_COLLECTABLE_BB_WIDTH * 0.75, PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT * 0.75};
			position = startPos + vec2(80.0f, 0.0f);
			keybindPos = position + vec2(-10.0f, 0.0f);
			iconPos = keybindPos + vec2(5.0f, -40.0f);
			textPos = iconPos + vec2(-30.0f, -55.0f);
			keybind = "2";
			break;
		case TEXTURE_ASSET_ID::BOW:
			iconScale = { BOW_BB_WIDTH * 0.60, BOW_BB_HEIGHT * 0.60};
			position = startPos + vec2(145.0f, 0.0f);
			keybindPos = position + vec2(-5.0f, 0.0f);
			iconPos = keybindPos + vec2(10.0f, -40.0f);
			textPos = iconPos + vec2(-30.0f, -55.0f);
			keybind = "3";
			break;
		default:
			break;
	}

	Text& textKeybind = registry.texts.emplace(textKeybindE);
	textKeybind.value = keybind;
	Foreground& fgKeybind = registry.foregrounds.emplace(textKeybindE);
	fgKeybind.position = keybindPos;
	fgKeybind.scale = {0.85f, 0.85f};
	registry.renderRequests.insert(
		textKeybindE, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	registry.texts.emplace(textCountE);
	Foreground& fg = registry.foregrounds.emplace(textCountE);
	fg.position = textPos;
	fg.scale = {1.2f, 1.2f};
	registry.colours.insert(textCountE, {0.8f, 0.8f, 0.0f, 1.0f});
	registry.renderRequests.insert(
		textCountE, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});

	Foreground& icon = registry.foregrounds.emplace(iconE);
	icon.scale = iconScale;
	icon.position = iconPos;
	registry.renderRequests.insert(
		iconE, 
		{
			assetID,
			EFFECT_ASSET_ID::TEXTURED_FLAT,
			GEOMETRY_BUFFER_ID::SPRITE
		});
	
	return textCountE;
}

Entity createMapTile(vec2 position, vec2 size, float height) {
    auto entity = Entity();
	registry.mapTiles.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(position, height);
	motion.scale = vec2(size.x, size.y * yConversionFactor);
	
    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::GRASS_TILE,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
		registry.backgrounds.emplace(entity);
	
    return entity;
}

void createObstacles() {
	std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist;
    rng = std::default_random_engine(std::random_device()());

	int numShrubs = 20;
	while(numShrubs != 0) {
		float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
		float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
		createNormalObstacle({posX, posY}, {SHRUB_BB_WIDTH, SHRUB_BB_HEIGHT}, TEXTURE_ASSET_ID::SHRUB);
		numShrubs--;
    }
	int numRocks = 15;
	while(numRocks != 0) {
		float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
		float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
		createNormalObstacle({posX, posY}, {ROCK_BB_WIDTH, ROCK_BB_HEIGHT}, TEXTURE_ASSET_ID::ROCK);
		numRocks--;
    }
}

Entity createObstacle(vec2 position, vec2 size, TEXTURE_ASSET_ID assetId) {
    auto entity = Entity();
    registry.obstacles.emplace(entity);

    Motion& motion = registry.motions.emplace(entity);
    motion.scale = size;

    motion.position = vec3(position.x, position.y, getElevation(position) + size.y / 2);
	motion.hitbox = { size.x, size.x, size.y / zConversionFactor };
	motion.solid = true;

    registry.renderRequests.insert(
        entity, 
        {
            assetId,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.midgrounds.emplace(entity);

    return entity;
}

Entity createNormalObstacle(vec2 position, vec2 size, TEXTURE_ASSET_ID assetId) {
    auto entity = Entity();
    registry.obstacles.emplace(entity);

    Motion& motion = registry.motions.emplace(entity);
    motion.scale = size;

    motion.position = vec3(position.x, position.y, getElevation(position) + size.y / 2);
	motion.hitbox = { size.x, size.x, size.y / zConversionFactor };
	motion.solid = true;

    registry.renderRequests.insert(
        entity, 
        {
            assetId,
            EFFECT_ASSET_ID::TEXTURED_NORMAL,
            GEOMETRY_BUFFER_ID::SPRITE,
			PRIMITIVE_TYPE::TRIANGLES
        });
    registry.midgrounds.emplace(entity);

    return entity;
}



Entity createBottomCliff(vec2 position, vec2 size) {
    auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(position, size.y / 2);
	motion.scale = vec2(size.x, size.y * yConversionFactor);
	motion.hitbox = { size.x, 1.9 * size.y, size.y };
	motion.solid = true;

	registry.obstacles.emplace(entity);

    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::CLIFF,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.midgrounds.emplace(entity); 
    return entity;
}

Entity createSideCliff(vec2 position, vec2 size) {
    auto entity = Entity();
	registry.mapTiles.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(position, size.y / 2);
	motion.scale = vec2(size.x, size.y * yConversionFactor);
	motion.hitbox = { abs(size.x) * 0.95, size.y, size.y };
	motion.solid = true;

	registry.obstacles.emplace(entity);

    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::CLIFFSIDE,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.midgrounds.emplace(entity); 
    return entity;
}
Entity createTopCliff(vec2 position, vec2 size) {
    auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(position, size.y / 2);
	motion.scale = vec2(size.x, size.y * yConversionFactor);
	motion.hitbox = { size.x, size.y / 16, size.y };
	motion.solid = true;

	registry.obstacles.emplace(entity);

    registry.renderRequests.insert(
        entity, 
        {
            TEXTURE_ASSET_ID::CLIFFTOP,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        });
    registry.midgrounds.emplace(entity);
    return entity;
}

void createCliffs(GLFWwindow* window) {
	float widthFactor = 0.5;
	for (int col = 1 / widthFactor; col < x_tiles / widthFactor; col++) {
		vec2 position = { (col - 0.5) * tile_x * widthFactor, tile_y };
		vec2 size = { tile_x * widthFactor, tile_y };
		createTopCliff(position, size);
	}
	for (int row = 0; row < y_tiles; row++) {
		vec2 position = { 0.3 * tile_x, (row + 1) * tile_y };
		vec2 size = { tile_x, tile_y };
		createSideCliff(position, size);
	}
	for (int row = 0; row < y_tiles; row++) {
		vec2 position = { (x_tiles - 0.3) * tile_x, (row + 1) * tile_y };
		vec2 size = { -tile_x, tile_y };
		createSideCliff(position, size);
	}
	for (int col = 1 / widthFactor; col < x_tiles / widthFactor; col++) {
		vec2 position = { (col - 0.5) * tile_x * widthFactor, y_tiles * tile_y };
		vec2 size = { tile_x * widthFactor, tile_y };
		createBottomCliff(position, size);
	}
}

void createMapTiles() {
    for (int row = 0; row < y_tiles; row++) { 
        for (int col = 0; col < x_tiles; col++) { 
            vec2 position = {(col + 0.5) * tile_x, (row + 0.5) * tile_y};
            vec2 size = {tile_x, tile_y};
			float height = 0;
            createMapTile(position, size, height);
        }
    }
}

void createHighScoreText(vec2 windowSize){
	auto entity = Entity();

	GameScore& gameScore = registry.gameScore;

	Text& text = registry.texts.emplace(entity);
    text.value = "Your High Score is  " + std::to_string(gameScore.highScoreHours) + "h " + std::to_string(gameScore.highScoreMinutes) + "m " + std::to_string(gameScore.highScoreSeconds) + "s ";
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = {windowSize.x / 2 - 220.f, windowSize.y / 2 - 50.f};
	fg.scale = {1.0f, 1.0f};
	
	registry.colours.insert(entity, {1.0f, 0.85f, 0.0f, 1.0f});

	registry.renderRequests.insert(
		entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});
}

void createGameOverText(vec2 windowSize) {
	auto backdrop = Entity();
	Foreground& backdropFg = registry.foregrounds.emplace(backdrop);
	backdropFg.position = {0.0f, 0.0f};
	backdropFg.scale = {windowSize.x, windowSize.y};

	registry.colours.insert(backdrop, vec4(0.0f, 0.0f, 0.0f, 0.6f));
	registry.renderRequests.insert(
		backdrop,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::UNTEXTURED,
			GEOMETRY_BUFFER_ID::RECTANGLE
		});

	GameTimer& gameTimer = registry.gameTimer;
	std::vector<Entity> entities;

	auto entity1 = Entity();
	Text& text1 = registry.texts.emplace(entity1);
	Foreground& text1Fg = registry.foregrounds.emplace(entity1);
	text1.value = "GAME OVER";
	text1Fg.position = {windowSize.x / 2 - 315.0f, windowSize.y / 2 + 50.0f};
	text1Fg.scale = {4.0f, 4.0f};
	registry.colours.insert(entity1, {0.85f, 0.0f, 0.0f, 1.0f});
	
	auto entity2 = Entity();
	Text& text2 = registry.texts.emplace(entity2);
	Foreground& text2Fg = registry.foregrounds.emplace(entity2);
	text2Fg.position = {windowSize.x / 2 - 160.f, windowSize.y / 2};
	text2Fg.scale = {1.0f, 1.0f};
	registry.colours.insert(entity2, {1.0f, 0.85f, 0.0f, 1.0f});
	std::ostringstream oss;
    oss << "You survived for ";
    if (gameTimer.hours > 0) {
        oss << gameTimer.hours << "h ";
		text2Fg.position.x -= 20.f;
    }
    if (gameTimer.minutes > 0 || gameTimer.hours > 0) {
        oss << gameTimer.minutes << "m ";
		text2Fg.position.x -= 40.f;
    }
    oss << gameTimer.seconds << "s";
	text2.value = oss.str();

	createHighScoreText(windowSize);

	auto entity3 = Entity();
	Text& text3 = registry.texts.emplace(entity3);
	text3.value = "Press ENTER to play again";
	Foreground& text3Fg = registry.foregrounds.emplace(entity3);
	text3Fg.position = {windowSize.x / 2 - 160.f, windowSize.y / 2 - 110.f};
	text3Fg.scale = {0.8f, 0.8f};

	entities.push_back(entity1);
	entities.push_back(entity2);
	entities.push_back(entity3);
	for (Entity& entity : entities) {
		registry.renderRequests.insert(
			entity, 
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});
	}

}

Entity createProjectile(vec3 pos, vec3 velocity, PROJECTILE_TYPE type)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = velocity;
	motion.scale = getProjectileInfo(type).size;
	motion.hitbox = { motion.scale.x, motion.scale.x, motion.scale.y / zConversionFactor };
	
	registry.projectiles.emplace(entity).type = type;
	registry.midgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			getProjectileInfo(type).assetId,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createMousePointer(vec2 mousePos) {
	auto entity = Entity();

	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.scale = { 40.0f, 40.0f};
	fg.position = mousePos;

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::CROSSHAIR,
			EFFECT_ASSET_ID::TEXTURED_FLAT,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

void createGameSaveText(vec2 windowSize) {
	auto entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.value = "Game Saved!";
	Foreground& fg = registry.foregrounds.emplace(entity);
	fg.position = { windowSize.x / 2 - 170.f, windowSize.y / 2 + 300.f };
	fg.scale = { 2.f, 2.f };

	Cooldown& cooldown = registry.cooldowns.emplace(entity);
	cooldown.remaining = 2000.f;

	registry.colours.insert(entity, { 0.0f, 1.0f, 0.0f, 1.0f });

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::NONE,
			EFFECT_ASSET_ID::FONT,
			GEOMETRY_BUFFER_ID::TEXT
		});
}

void createTrees(RenderSystem* renderer) {
	int numTrees = 4;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	rng = std::default_random_engine(std::random_device()());

	while (numTrees != 0) {
		float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
		float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
		createTree(renderer, { posX, posY });
		numTrees--;
	}
}

ProjectileInfo getProjectileInfo(PROJECTILE_TYPE type) {
    switch (type) {
        case PROJECTILE_TYPE::TRAP:
            return { {TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_HEIGHT}, 
                     TEXTURE_ASSET_ID::TRAPCOLLECTABLE };
        case PROJECTILE_TYPE::PHANTOM_TRAP:
            return { {PHANTOM_TRAP_COLLECTABLE_BB_WIDTH, PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT}, 
                     TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE };
        case PROJECTILE_TYPE::ARROW:
            return { {ARROW_BB_WIDTH, ARROW_BB_HEIGHT}, 
                     TEXTURE_ASSET_ID::ARROW };
        default:
            return { {0, 0}, TEXTURE_ASSET_ID::NONE };
    }
}
float getElevation(vec2 xy)
{
	return 0.0f;
}
