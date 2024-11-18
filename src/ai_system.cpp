#include "ai_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include "sound_system.hpp"

//Boar constants
const float BOAR_AGGRO_RANGE = 500;
const float BOAR_DISENGAGE_RANGE = 700;
const float BOAR_PREPARE_TIME = 500;
const float BOAR_CHARGE_DURATION = 1000;
const float BOAR_COOLDOWN_TIME = 500;
const float BOAR_CHARGE_SPEED = 1.0f;

//Bird constants
const float BIRD_AGGRO_RANGE = 100;   
const float BIRD_SWOOP_DURATION = 500; 
const float BIRD_COOLDOWN_TIME = 4000;


AISystem::AISystem(std::default_random_engine& rng, SoundSystem* sound)
{
    this->rng = rng;
	this->sound = sound;
}

vec2 AISystem::randomDirection()
{
    float angle = uniform_dist(rng) * 2 * M_PI;
    return vec2(cos(angle), sin(angle));
}

void AISystem::moveTowardsPlayer(Entity enemy, vec3 playerPosition, float elapsed_ms)
{
    Motion& enemyMotion = registry.motions.get(enemy);

    // Skip if in the air
    if (enemyMotion.position.z - enemyMotion.hitbox.z / 2 > getElevation(vec2(enemyMotion.position)) + 1) {
        return;
    }
    
    float& pathfindTime = registry.enemies.get(enemy).pathfindTime;
    pathfindTime -= elapsed_ms;
    if (pathfindTime > 0) {
        return;
    }
    pathfindTime = 100 + uniform_dist(rng) * 400;

    vec2 direction = chooseDirection(enemyMotion, playerPosition);
    enemyMotion.facing = direction;
    float speed = registry.enemies.get(enemy).speed;
    enemyMotion.velocity = vec3(direction * speed, enemyMotion.velocity.z);
}

vec2 AISystem::chooseDirection(Motion& motion, vec3 playerPosition)
{
    const vec2 playerDirection = normalize(playerPosition - motion.position);

    const unsigned int NUMBER_OF_DIRECTIONS = 60;
    const float OFFSET = 2 * M_PI / NUMBER_OF_DIRECTIONS;
    float radius = 400;

    float d = distance(motion.position, playerPosition);
    if (d < radius) {
        radius = d;
    }

    std::vector<Entity>& allObstacles = registry.obstacles.entities;
    std::vector<Entity> obstacles;
    // only include obstacles within the range we care about
    // won't work for extremely large obstacles (where none of their hitbox vertices will be inside the radius)
    std::copy_if(allObstacles.begin(), allObstacles.end(), std::back_inserter(obstacles), 
        [&motion, radius](Entity obstacle) {
            Motion& obstacleMotion = registry.motions.get(obstacle);
            std::vector<vec3> vertices = boundingBoxVertices(obstacleMotion);
            for (auto& vertex : vertices) {
                float d = distance(motion.position, vertex);
                if (d < radius)
                    return true;
            }
            return false;
        }
    );

    vec2 bestDirection = playerDirection;
    float bestClearDistance = 0;
    for (unsigned int i = 0; i < NUMBER_OF_DIRECTIONS; i++) {
        int side = i % 2 == 0 ? -1 : 1;     // which side to apply offset
        vec2 direction = rotate(playerDirection, OFFSET * ceil(i / 2.f) * side);
        float clearDistance;
        if (pathClear(motion, direction, radius, obstacles, clearDistance)) {
            return direction;
        }
        if (clearDistance > bestClearDistance) {
            bestDirection = direction;
            bestClearDistance = bestClearDistance;
        }
    }

    if (bestClearDistance < 100) {
        return randomDirection();
    }

    return bestDirection;
}

// Uses hitbox vertices except for the vertex in the direction quadrant 
static std::vector<vec2> pathPolygon(Motion& motion, vec2 pathEnd)
{
    std::vector<vec2> polygon;
    vec2 topRight = vec2(motion.position) + vec2(motion.hitbox.x, motion.hitbox.y) / 2.f;
    vec2 topLeft  = vec2(motion.position) + vec2(-motion.hitbox.x, motion.hitbox.y) / 2.f;
    vec2 botRight = vec2(motion.position) + vec2(motion.hitbox.x, -motion.hitbox.y) / 2.f;
    vec2 botLeft  = vec2(motion.position) + vec2(-motion.hitbox.x, -motion.hitbox.y) / 2.f;
    if (pathEnd.x >= 0 && pathEnd.y <= 0) {
        polygon.push_back(topLeft);
        polygon.push_back(botLeft);
        polygon.push_back(botRight);
        polygon.push_back(botRight + pathEnd);
        polygon.push_back(topRight + pathEnd);
        polygon.push_back(topLeft  + pathEnd);
    }
    else if (pathEnd.x >= 0 && pathEnd.y >= 0) {
        polygon.push_back(topRight);
        polygon.push_back(topLeft);
        polygon.push_back(botLeft);
        polygon.push_back(botLeft  + pathEnd);
        polygon.push_back(botRight + pathEnd);
        polygon.push_back(topRight + pathEnd);
    }
    else if (pathEnd.x <= 0 && pathEnd.y <= 0) {
        polygon.push_back(botRight);
        polygon.push_back(topRight);
        polygon.push_back(topLeft);
        polygon.push_back(topLeft  + pathEnd);
        polygon.push_back(botLeft  + pathEnd);
        polygon.push_back(botRight + pathEnd);
    }
    else if (pathEnd.x <= 0 && pathEnd.y >= 0) {
        polygon.push_back(botLeft);
        polygon.push_back(botRight);
        polygon.push_back(topRight);
        polygon.push_back(topRight + pathEnd);
        polygon.push_back(topLeft  + pathEnd);
        polygon.push_back(botLeft  + pathEnd);
    }
    return polygon;
}

// Returns whether the path is clear or not
// If path is not clear, sets clearDistance to the distance along the path that is clear
bool AISystem::pathClear(Motion& motion, vec2 direction, float howFar, const std::vector<Entity>& obstacles, float& clearDistance)
{
    // Filter out obstacles that are outside of the Z range
    std::vector<Entity> verticallyBlockingObstacles;
    std::copy_if(obstacles.begin(), obstacles.end(), std::back_inserter(verticallyBlockingObstacles),
        [&motion](Entity obstacle) {
            Motion& obstacleMotion = registry.motions.get(obstacle);
            return !(
                obstacleMotion.position.z - obstacleMotion.hitbox.z / 2 > motion.position.z + motion.hitbox.z / 2 ||
                obstacleMotion.position.z + obstacleMotion.hitbox.z / 2 < motion.position.z - motion.hitbox.z / 2);
        }
    );

    // Horizontal path polygon
    std::vector<vec2> polygon = pathPolygon(motion, direction * howFar);
    
    // Obstacles that block in both horizontal and vertical ranges
    std::vector<Entity> blockingObstacles;
    std::copy_if(verticallyBlockingObstacles.begin(), verticallyBlockingObstacles.end(), std::back_inserter(blockingObstacles),
        [&polygon](Entity obstacle) {
            float hitboxFactor = 1;
            if (registry.meshPtrs.has(obstacle)) {
                hitboxFactor = 0.2;
            }

            Motion& obstacleMotion = registry.motions.get(obstacle);
            std::vector<vec2> obstaclePolygon;
            vec2 centre = vec2(obstacleMotion.position);
            obstaclePolygon.push_back(centre + vec2( obstacleMotion.hitbox.x,  obstacleMotion.hitbox.y) / 2.f * hitboxFactor);
            obstaclePolygon.push_back(centre + vec2(-obstacleMotion.hitbox.x,  obstacleMotion.hitbox.y) / 2.f * hitboxFactor);
            obstaclePolygon.push_back(centre + vec2(-obstacleMotion.hitbox.x, -obstacleMotion.hitbox.y) / 2.f * hitboxFactor);
            obstaclePolygon.push_back(centre + vec2( obstacleMotion.hitbox.x, -obstacleMotion.hitbox.y) / 2.f * hitboxFactor);
            return polygonsCollide(polygon, obstaclePolygon);
        });

    if (blockingObstacles.size() == 0) {
        return true;
    }

    float minDistance = FLT_MAX;
    for (Entity obstacle : blockingObstacles) {
        Motion& obstacleMotion = registry.motions.get(obstacle);
        float d = distance(obstacleMotion.position, motion.position);
        if (d < minDistance) {
            minDistance = d;
        }
    }
    clearDistance = minDistance;

    return false;
}



void AISystem::boarBehaviour(Entity boar, vec3 playerPosition, float elapsed_ms)
{
    // boar can't charge if trapped
    if(registry.enemies.has(boar) && registry.enemies.get(boar).isTrapped) {
        moveTowardsPlayer(boar, playerPosition, elapsed_ms);
        return;
    }

    if (registry.deathTimers.has(boar)) {
        return;
    }

    Motion& motion = registry.motions.get(boar);
    Boar& boars = registry.boars.get(boar);
    AnimationController& animationController = registry.animationControllers.get(boar);
    float distanceToPlayer = distance(motion.position, playerPosition);
    vec2 directionToPlayer = normalize(vec2(playerPosition) - vec2(motion.position));

    if (boars.cooldownTimer > 0) {
        boars.cooldownTimer -= elapsed_ms;
        return;
    }

    // Set state based on distance
    float clearDistance;
    if (distanceToPlayer < BOAR_AGGRO_RANGE && boars.cooldownTimer <= 0 && !boars.preparing && !boars.charging &&
            pathClear(motion, directionToPlayer, distanceToPlayer, registry.obstacles.entities, clearDistance)) {
        boars.preparing = true;
        boars.prepareTimer = BOAR_PREPARE_TIME;
        boars.chargeTimer = BOAR_CHARGE_DURATION;
        animationController.changeState(boar, AnimationState::Idle);
    } else if (distanceToPlayer > BOAR_DISENGAGE_RANGE) {
        boars.preparing = false;
        boars.charging = false;
    }

    if (boars.preparing) {
        // Preparation shake
        motion.facing = directionToPlayer;
        if (boars.prepareTimer > 0) {
            boars.prepareTimer -= elapsed_ms;

            float shakeMagnitude = 5.0f;
            float offsetX = (uniform_dist(rng) - 0.5f) * shakeMagnitude;
            float offsetY = (uniform_dist(rng) - 0.5f) * shakeMagnitude;

            motion.position.x += offsetX;
            motion.position.y += offsetY;
            motion.velocity = vec3(0, 0, 0);

        } else {
            boars.preparing = false;
            if (pathClear(motion, directionToPlayer, distanceToPlayer, registry.obstacles.entities, clearDistance)) {
                animationController.changeState(boar, AnimationState::Running);
                boars.charging = true;
                boars.chargeDirection = directionToPlayer;
                motion.velocity = vec3(boars.chargeDirection * BOAR_CHARGE_SPEED, 0);
                sound->playSoundEffect(sound->BOAR_CHARGE_SOUND, audio_path("boar_charge.wav"), 0);
            }
        }
    }

    if (boars.charging) {
        if (boars.chargeTimer > 0) {
            boars.chargeTimer -= elapsed_ms;
        } 
        else {
            boarReset(boar);
        }
    } 
    else if (!boars.preparing) {
        moveTowardsPlayer(boar, playerPosition, elapsed_ms);
        animationController.changeState(boar, AnimationState::Running);
    }
}

void AISystem::boarReset(Entity boar)
{
    AnimationController& animationController = registry.animationControllers.get(boar);
    Motion& motion = registry.motions.get(boar);
    Boar& boars = registry.boars.get(boar);
    animationController.changeState(boar, AnimationState::Idle);
    boars.charging = false;
    boars.cooldownTimer = BOAR_COOLDOWN_TIME;
    motion.velocity = vec3(0, 0, 0);
}

void AISystem::barbarianBehaviour(Entity barbarian, vec3 playerPosition, float elapsed_ms)
{
    if (registry.deathTimers.has(barbarian)) {
        return;
    }
    moveTowardsPlayer(barbarian, playerPosition, elapsed_ms);
}

void AISystem::shootArrow(Entity shooter, vec3 targetPos)
{
    // Always shoot arrow at 45 degree angle (makes calculations simpler)
    const float ARROW_ANGLE = M_PI / 4;
    const float MAX_ARROW_VELOCITY = 10;

    Motion& motion = registry.motions.get(shooter);

    // Get start position of the arrow
    vec2 horizontal_direction = normalize(vec2(targetPos) - vec2(motion.position));
    const float maxArrowDimension = max(ARROW_BB_HEIGHT, ARROW_BB_WIDTH);
    vec3 pos = motion.position;
    if (abs(horizontal_direction.x) > abs(horizontal_direction.y)) {
        if (horizontal_direction.x > 0) {
            pos.x += motion.hitbox.x / 2 + maxArrowDimension;
        }
        else if (horizontal_direction.x < 0) {
            pos.x -= motion.hitbox.x / 2 + maxArrowDimension;
        }
    }
    else {
        if (horizontal_direction.y > 0) {
            pos.y += motion.hitbox.y / 2 + maxArrowDimension;
        }
        else if (horizontal_direction.x < 0) {
            pos.y -= motion.hitbox.y / 2 + maxArrowDimension;
        }
    }
    pos.z += motion.hitbox.z / 2 + maxArrowDimension;
    horizontal_direction = normalize(vec2(targetPos) - vec2(pos));

    // Get distances from start to target
    float horizontal_distance = distance(vec2(pos), vec2(targetPos));
    float vertical_distance = targetPos.z - pos.z;

    // Prevent trying to shoot above what is possible
    if (vertical_distance >= horizontal_distance)
        return;

    float velocity = horizontal_distance * sqrt(-GRAVITATIONAL_CONSTANT / (vertical_distance - horizontal_distance));

    // Prevent shooting at crazy speeds
    if (velocity > MAX_ARROW_VELOCITY)
        return;

    // Introduce some randomness in the velocity 
    float random_factor = 1 + (0.5 - uniform_dist(rng)) / 20; // 0.975-1.025
    velocity *= random_factor;

    // Determine velocities for each dimension
    vec2 horizontal_velocity = velocity * cos(ARROW_ANGLE) * horizontal_direction;
    float vertical_velocity = velocity * sin(ARROW_ANGLE);
    createArrow(pos, vec3(horizontal_velocity, vertical_velocity));
	sound->playSoundEffect(sound->ARROW_SOUND,audio_path("arrow.wav"), 0);
}


void AISystem::archerBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms)
{
    const float ARCHER_RANGE = 600;
    const float DISENGAGE_RANGE = 800;
    const float DRAW_ARROW_TIME = 1000;
    if (registry.deathTimers.has(entity)) {
        return;
    }
    Motion& motion = registry.motions.get(entity);
    Archer& archer = registry.archers.get(entity);
    float d = distance(motion.position, playerPosition);
    if (d < ARCHER_RANGE) {
        archer.aiming = true;
        motion.velocity.x = 0;
        motion.velocity.y = 0;
        AnimationController& animationController = registry.animationControllers.get(entity);
        animationController.changeState(entity, AnimationState::Attack);
    }
    else if (d > DISENGAGE_RANGE) {
        archer.aiming = false;
        archer.drawArrowTime = 0;
        AnimationController& animationController = registry.animationControllers.get(entity);
        animationController.changeState(entity, AnimationState::Running);
    }

    if (archer.aiming) {
        motion.facing = normalize(vec2(playerPosition) - vec2(motion.position));
        if (archer.drawArrowTime > DRAW_ARROW_TIME) {
            shootArrow(entity, playerPosition);
            archer.drawArrowTime = 0;
            AnimationController& animationController = registry.animationControllers.get(entity);
            animationController.changeState(entity, AnimationState::Idle);
        }
        else {
            archer.drawArrowTime += elapsed_ms;
        }
    }
    else {
        moveTowardsPlayer(entity, playerPosition, elapsed_ms);
    }
}

// steer to avoid crowding local flockmates
vec2 separation(const Motion& motion, const std::vector<Motion>& flockMates) {
    vec2 separationForce = vec2(0, 0);
    const float SEPARATION_RADIUS = 300.0f;  
    const float SEPARATION_WEIGHT = 5.0f;   

    for (const auto& mate : flockMates) {
        float distanceToMate = distance(motion.position, mate.position);
        if (distanceToMate > 0 && distanceToMate < SEPARATION_RADIUS) {
            vec2 directionAway = normalize(vec2(motion.position) - vec2(mate.position));
            separationForce += directionAway / distanceToMate; 
        }
    }

    return separationForce * SEPARATION_WEIGHT;
}

// steer towards the average heading of local flockmates
vec2 alignment(const Motion& motion, const std::vector<Motion>& flockMates) {
    vec2 alignmentForce = vec2(0, 0);
    const float ALIGNMENT_RADIUS = 10.0f;  
    const float ALIGNMENT_WEIGHT = 1.0f;    

    for (const auto& mate : flockMates) {
        float distanceToMate = glm::length(motion.position - mate.position); 

        if (distanceToMate > 0 && distanceToMate < ALIGNMENT_RADIUS) {
            alignmentForce += vec2(mate.velocity.x, motion.velocity.y);
        }
    }

    return alignmentForce * ALIGNMENT_WEIGHT;
}

// steer to move toward the average position of local flockmates
vec2 cohesion(const Motion& motion, const std::vector<Motion>& flockMates) {
    vec2 cohesionForce = vec2(0, 0);
    const float COHESION_RADIUS = 10.0f; 
    const float COHESION_WEIGHT = 1.0f;    

    for (const auto& mate : flockMates) {
        float distanceToMate = glm::length(motion.position - mate.position);
        
        if (distanceToMate > 0 && distanceToMate < COHESION_RADIUS) {
            cohesionForce += vec2(mate.position.x, mate.position.y) - vec2(motion.position.x, motion.position.y);
        }
    }

    return cohesionForce * COHESION_WEIGHT;
}

void AISystem::swoopAttack(Entity bird, vec3 playerPosition, float elapsed_ms, const std::vector<Motion>& flockMates) {
    Motion& birdMotion = registry.motions.get(bird);
    Bird& birdComponent = registry.birds.get(bird);
    AnimationController& animationController = registry.animationControllers.get(bird);

    // Initiate swoop when within range
    vec2 birdPosition2D = vec2(birdMotion.position.x, birdMotion.position.y);
    vec2 playerPosition2D = vec2(playerPosition.x, playerPosition.y);
    float distanceToPlayer = distance(birdPosition2D, playerPosition2D);

    if (!birdComponent.isSwooping && birdComponent.swoopCooldown <= 0 && distanceToPlayer < BIRD_AGGRO_RANGE) {
        birdComponent.isSwooping = true;
        birdComponent.swoopTimer = BIRD_SWOOP_DURATION;
        birdComponent.swoopDirection = normalize(playerPosition2D - birdPosition2D);
    }

    // Swoop towards player
    if (birdComponent.isSwooping) {
        animationController.changeState(bird, AnimationState::Swooping);

		if (birdComponent.swoopTimer == BIRD_SWOOP_DURATION) {
			sound->playSoundEffect(sound->BIRD_ATTACK_SOUND, audio_path("bird_attack.wav"), 0);
		}

        // BOID while swooping
        vec2 separationForce = separation(birdMotion, flockMates) * 0.5f;
        vec2 alignmentForce = alignment(birdMotion, flockMates) * 0.3f;
        vec2 cohesionForce = cohesion(birdMotion, flockMates) * 0.5f;
        vec2 swoopForce = birdComponent.swoopDirection * birdComponent.swoopSpeed;
        vec2 combinedForce = swoopForce + separationForce + alignmentForce + cohesionForce;

        birdMotion.velocity = vec3(normalize(combinedForce) * birdComponent.swoopSpeed, -1.0f);

        birdComponent.swoopTimer -= elapsed_ms;
        if (birdComponent.swoopTimer <= 0) {
            if (birdMotion.position.z < birdComponent.originalZ) {
                animationController.changeState(bird, AnimationState::Flying);
                birdMotion.velocity.z = 1.0f;
            } else {
                birdMotion.position.z = birdComponent.originalZ;
                birdComponent.isSwooping = false;
                birdMotion.velocity.z = 0;
                birdComponent.swoopCooldown = BIRD_COOLDOWN_TIME;
            }
        }
    }
}

void AISystem::birdBehaviour(Entity bird, vec3 playerPosition, float elapsed_ms) {
    Motion& birdMotion = registry.motions.get(bird);
    Bird& birdComponent = registry.birds.get(bird);
    AnimationController& animationController = registry.animationControllers.get(bird);
    std::vector<Motion> flockMates;
    for (const auto& entity : registry.birds.entities) {
        if (entity.getId() != bird.getId() && registry.motions.has(entity)) {
            flockMates.push_back(registry.motions.get(entity));
        }
    }
    // Swoop Attack
    swoopAttack(bird, playerPosition, elapsed_ms, flockMates);
    if (birdComponent.isSwooping) {
        return;
    }
    if (birdComponent.swoopCooldown > 0) {
        birdComponent.swoopCooldown -= elapsed_ms;
    }

    // BOIDS: Separation, Alignment, Cohesion
    vec2 separationForce = separation(birdMotion, flockMates);
    vec2 alignmentForce = alignment(birdMotion, flockMates);
    vec2 cohesionForce = cohesion(birdMotion, flockMates);
    vec2 birdPosition2D = vec2(birdMotion.position.x, birdMotion.position.y);
    vec2 playerPosition2D = vec2(playerPosition.x, playerPosition.y);
    float distanceToPlayer = distance(birdPosition2D, playerPosition2D);

    vec2 directionToPlayer = normalize(playerPosition2D - birdPosition2D);
    const float SEPARATION_WEIGHT = 0.5f;
    const float ALIGNMENT_WEIGHT = 0.3f;
    const float COHESION_WEIGHT = 0.5f;
    const float PLAYER_ATTRACTION_WEIGHT = 0.2f;
    
    vec2 flockingForce = 
        separationForce * SEPARATION_WEIGHT + 
        alignmentForce * ALIGNMENT_WEIGHT + 
        cohesionForce * COHESION_WEIGHT;
    
    vec2 movementForce = flockingForce + directionToPlayer * PLAYER_ATTRACTION_WEIGHT;
    float speed = registry.birds.get(bird).swarmSpeed;
    if (length(movementForce) > 0) {
        movementForce = normalize(movementForce) * speed;
    }
    animationController.changeState(bird, AnimationState::Flying);
    birdMotion.velocity = vec3(movementForce, 0.0f);
    birdMotion.facing = normalize(movementForce);
}

void AISystem::wizardBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms) {
    
	if (registry.deathTimers.has(entity)) {
		return;
	}

	Wizard& wizard = registry.wizards.get(entity);
    switch (wizard.state) {
    case WizardState::Moving:
        processWizardMoving(entity, playerPosition, elapsed_ms);
        break;
    case WizardState::Aiming:
		processWizardAiming(entity, playerPosition, elapsed_ms);
        break;
	case WizardState::Preparing:
        processWizardPreparing(entity, playerPosition, elapsed_ms);
        break;
	case WizardState::Shooting:
		processWizardShooting(entity, playerPosition, elapsed_ms);
		break;
    default:
        break;
    }
}

void AISystem::processWizardMoving(Entity entity, vec3 playerPosition, float elapsed_ms) {
    const float WIZARD_RANGE = 600;

    Motion& motion = registry.motions.get(entity);
    Wizard& wizard = registry.wizards.get(entity);
    AnimationController& animationController = registry.animationControllers.get(entity);
    float d = distance(motion.position, playerPosition);

    if (d < WIZARD_RANGE) {
        wizard.state = WizardState::Aiming;
        motion.velocity.x = 0;
        motion.velocity.y = 0;
        animationController.changeState(entity, AnimationState::Idle);
    }
    else {
        moveTowardsPlayer(entity, playerPosition, elapsed_ms);
    }
}

void AISystem::processWizardAiming(Entity entity, vec3 playerPosition, float elapsed_ms) {
	const float EDGE_BUFFER = 500;

    float rand = uniform_dist(rng);
    Motion& motion = registry.motions.get(entity);
    Wizard& wizard = registry.wizards.get(entity);
    motion.facing = normalize(vec2(playerPosition) - vec2(motion.position));

    bool farFromEdge =
        playerPosition.x > leftBound + EDGE_BUFFER &&
        playerPosition.x < rightBound - EDGE_BUFFER &&
        playerPosition.y > topBound + EDGE_BUFFER &&
        playerPosition.y < bottomBound - EDGE_BUFFER;

    // calculate if path is clear
	vec2 direction = normalize(vec2(playerPosition) - vec2(motion.position));
	float howFar = distance(motion.position, playerPosition);
	std::vector<Entity> obstacles = registry.obstacles.entities;
    float clearDistance;
    bool clear = pathClear(motion, direction, howFar, obstacles, clearDistance);

	// face the shooter towards the player
    motion.facing = direction;

    // choose a random attack (fireball OR lightning)
    if (rand < 0.5 && clear) {
		shootFireball(entity, playerPosition);
		wizard.shoot_cooldown = 0;
		wizard.state = WizardState::Shooting;
	}
	else if (farFromEdge) {
		// start preparing for lightning
		createTargetArea(playerPosition, LIGHTNING_RADIUS);
		wizard.locked_target = playerPosition;
		wizard.state = WizardState::Preparing;
    }
    else {
        shootFireball(entity, playerPosition);
        wizard.shoot_cooldown = 0;
        wizard.state = WizardState::Shooting;
    }
}

void AISystem::processWizardPreparing(Entity entity, vec3 playerPosition, float elapsed_ms) {
    const float LIGHTNING_PREPARE_TIME = 1500;

    Wizard& wizard = registry.wizards.get(entity);

    // make a decision to trigger lightning or keep preparing
    if (wizard.prepareLightningTime > LIGHTNING_PREPARE_TIME) {
        triggerLightning(wizard.locked_target);
        wizard.state = WizardState::Shooting;
        wizard.prepareLightningTime = 0;
        wizard.shoot_cooldown = 0;
    }
    else {
        if (wizard.prepareLightningTime == 0) {
			sound->playSoundEffect(sound->STORM_SOUND, audio_path("storm.wav"), 0);
        }
        wizard.prepareLightningTime += elapsed_ms;
    }
}

void AISystem::processWizardShooting(Entity entity, vec3 playerPosition, float elapsed_ms) {
    const float SHOT_COOLDOWN = 5000;

	Motion& motion = registry.motions.get(entity);
	vec2 direction = normalize(vec2(playerPosition) - vec2(motion.position));
    motion.facing = direction;

    Wizard& wizard = registry.wizards.get(entity);

    if (wizard.shoot_cooldown > SHOT_COOLDOWN) {
        wizard.state = WizardState::Moving;
    }
    else {
        wizard.shoot_cooldown += elapsed_ms;
    }
}

void AISystem::shootFireball(Entity shooter, vec3 targetPos) {
    // Shoot in a straight line towards the player
    const float FIREBALL_SPEED = 0.5f;

    Motion& motion = registry.motions.get(shooter);

    // Direction to the player
    vec2 direction = normalize(vec2(targetPos) - vec2(motion.position));

    // Start position of the fireball
    vec3 pos = motion.position;
    // Set offset to avoid collision with the shooter
    float x_offset = FIREBALL_HITBOX_WIDTH + motion.hitbox.x / 2;
    float y_offset = FIREBALL_HITBOX_WIDTH + motion.hitbox.y / 2;
	// travelling more horizontally so no y offset
    if (abs(direction.x) > abs(direction.y)) {
        y_offset = 0;
    }
    else if (abs(direction.x) < abs(direction.y)) {
        x_offset = 0;
    }
    // offset must be on the left if travelling left
    if (direction.x < 0) {
        x_offset = -x_offset;
    }
    if (direction.y < 0) {
        y_offset = -y_offset;
    }
    pos += vec3(x_offset, y_offset, 0);

    direction = normalize(vec2(targetPos) - vec2(pos));

    // Velocity of the fireball
    vec3 velocity = vec3(direction * FIREBALL_SPEED, 0);

    createFireball(pos, direction);
	sound->playSoundEffect(sound->FIREBALL_SOUND, audio_path("fireball.wav"), 0);
}

void AISystem::triggerLightning(vec3 target_pos) {
    const float LIGHTNING_COUNT = 3;
    sound->stopSoundEffect(sound->STORM_SOUND);
	sound->playSoundEffect(sound->THUNDER_SOUND, audio_path("thunder.wav"), 0);
    for (int i = 0; i < LIGHTNING_COUNT; i++) {
		float angle = uniform_dist(rng) * 2 * M_PI;
		float radius = uniform_dist(rng) * LIGHTNING_RADIUS;

		float x = radius * cos(angle);
		float y = radius * sin(angle);
		vec3 pos = target_pos + vec3(x, y, 0);

		createLightning(pos);
    }
}

void AISystem::step(float elapsed_ms)
{
    // Skip if there is no player to pursue
    if (registry.players.entities.size() < 1) {
        return;
    }
    vec3 playerPosition = registry.motions.get(registry.players.entities.at(0)).position;
    for (Entity enemy : registry.enemies.entities) {
        if (registry.boars.has(enemy)) {
            boarBehaviour(enemy, playerPosition, elapsed_ms);
        }
        else if (registry.barbarians.has(enemy)) {
            barbarianBehaviour(enemy, playerPosition, elapsed_ms);
        }
        else if (registry.archers.has(enemy)) {
            archerBehaviour(enemy, playerPosition, elapsed_ms);
        } else if (registry.birds.has(enemy)){
            birdBehaviour(enemy, playerPosition, elapsed_ms);
        }
		else if (registry.wizards.has(enemy)) {
			wizardBehaviour(enemy, playerPosition, elapsed_ms);
		}
    }
}
