#include "ai_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include "world_system.hpp"

AISystem::AISystem(std::default_random_engine& rng)
{
    this->rng = rng;
}

vec2 AISystem::randomDirection()
{
    float angle = uniform_dist(rng) * 2 * M_PI;
    return vec2(cos(angle), sin(angle));
}

void AISystem::moveTowardsPlayer(Entity enemy, vec3 playerPosition, float elapsed_ms)
{
    float& pathfindTime = registry.enemies.get(enemy).pathfindTime;
    pathfindTime -= elapsed_ms;
    if (pathfindTime > 0) {
        return;
    }
    pathfindTime = 100 + uniform_dist(rng) * 400;

    Motion& enemyMotion = registry.motions.get(enemy);
    vec2 direction = chooseDirection(enemyMotion, playerPosition);
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
                hitboxFactor = 0.5;
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
    const float BOAR_AGGRO_RANGE = 500;
    const float BOAR_DISENGAGE_RANGE = 700;
    const float BOAR_PREPARE_TIME = 500; 
    const float BOAR_CHARGE_DURATION = 1000; 
    const float BOAR_COOLDOWN_TIME = 500; 
    const float BOAR_CHARGE_SPEED = 1.0f; 

    if (registry.deathTimers.has(boar)) {
        return;
    }

    Motion& motion = registry.motions.get(boar);
    Boar& boars = registry.boars.get(boar);
    float distanceToPlayer = distance(motion.position, playerPosition);

    if (boars.cooldownTimer > 0) {
        boars.cooldownTimer -= elapsed_ms;
        return;
    }

    // Set state based on distance
    if (distanceToPlayer < BOAR_AGGRO_RANGE && boars.cooldownTimer <= 0 && !boars.preparing && !boars.charging) {
        boars.preparing = true;
        boars.prepareTimer = BOAR_PREPARE_TIME;
        boars.chargeTimer = BOAR_CHARGE_DURATION;
    } else if (distanceToPlayer > BOAR_DISENGAGE_RANGE) {
        boars.preparing = false;
        boars.charging = false;
    }

    if (boars.preparing) {
        // Preparation shake
        if (boars.prepareTimer > 0) {
            AnimationController& animationController = registry.animationControllers.get(boar);
            animationController.changeState(boar, AnimationState::Idle);
            boars.prepareTimer -= elapsed_ms;

            float shakeMagnitude = 5.0f;
            float offsetX = (uniform_dist(rng) - 0.5f) * shakeMagnitude;
            float offsetY = (uniform_dist(rng) - 0.5f) * shakeMagnitude;

            motion.position.x += offsetX;
            motion.position.y += offsetY;
            motion.velocity = vec3(0, 0, 0);

        } else {
            AnimationController& animationController = registry.animationControllers.get(boar);
            animationController.changeState(boar, AnimationState::Running);
            boars.preparing = false; 
            boars.charging = true;
            boars.chargeDirection = normalize(vec2(playerPosition) - vec2(motion.position));
            motion.velocity = vec3(boars.chargeDirection * BOAR_CHARGE_SPEED, 0);
        }
    }

    if (boars.charging) {
        AnimationController& animationController = registry.animationControllers.get(boar);
        if (boars.chargeTimer > 0) {
            animationController.changeState(boar, AnimationState::Running);
            boars.chargeTimer -= elapsed_ms;

        } else {
            animationController.changeState(boar, AnimationState::Idle);
            boars.charging = false;
            boars.cooldownTimer = BOAR_COOLDOWN_TIME;
            motion.velocity = vec3(0, 0, 0);
        }
    } else if (!boars.preparing) {
        moveTowardsPlayer(boar, playerPosition, elapsed_ms);
    }
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
        }
    }
}
