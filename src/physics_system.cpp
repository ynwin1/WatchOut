#include "physics_system.hpp"
#include "world_init.hpp"

bool collides(const Hitbox& a, const Hitbox& b)
{
	// position represents the center of the entity
	// dimension represents the width and height of entity
	
	// If a's bottom is higher than b's top
	if (a.position.y > b.position.y + (b.dimension.y / 2.0f))
		return false;
	// If a's top is lower than b's bottom
	if (a.position.y + (a.dimension.y / 2.0f) < b.position.y)
		return false;
	// If a's right is to the left of b's left
	if (a.position.x > b.position.x + (b.dimension.x / 2.0f))
		return false;
	// Check if a's left is to the right of b's right
	if (a.position.x + (a.dimension.x / 2.0f) < b.position.x)
		return false;

	return true;
}