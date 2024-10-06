#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"
#include "render_components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ComponentContainer<Player> players;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Collectible> collectibles;
	ComponentContainer<Hitbox> hitboxes;
	ComponentContainer<Trap> traps;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<Animation> animations;

	ECSRegistry()
	{
		registry_list.push_back(&players);
		registry_list.push_back(&enemies);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&collectibles);
		registry_list.push_back(&hitboxes);
		registry_list.push_back(&traps);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&animations);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;