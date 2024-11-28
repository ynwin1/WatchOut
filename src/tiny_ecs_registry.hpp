#pragma once
#include <vector>
#include <map>

#include "tiny_ecs.hpp"
#include "components.hpp"
#include "render_components.hpp"
#include "animation_system.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ComponentContainer<Player> players;
	ComponentContainer<Dash> dashers;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Cooldown> cooldowns;
	ComponentContainer<Collectible> collectibles;
	ComponentContainer<Trap> traps;
	ComponentContainer<Damaged> damageds;
	ComponentContainer<Damaging> damagings;
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Knockable> knockables;
	ComponentContainer<Knocker> knockers;
	ComponentContainer<Trappable> trappables;
	ComponentContainer<HealthBar> healthBars;
	ComponentContainer<AnimationController> animationControllers;
	ComponentContainer<StaminaBar> staminaBars;
	ComponentContainer<Stamina> staminas;
	ComponentContainer<Text> texts;
	ComponentContainer<Jumper> jumpers;
	ComponentContainer<MapTile> mapTiles;
	ComponentContainer<Obstacle> obstacles;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<TargetArea> targetAreas;
	ComponentContainer<Collected> collected;
	
	ComponentContainer<PauseMenuComponent> pauseMenuComponents;
	ComponentContainer<HelpMenuComponent> helpMenuComponents;
	ComponentContainer<TutorialComponent> tutorialComponents;
	ComponentContainer<EnemyTutorialComponents> enemyTutorialComponents;
	ComponentContainer<CollectibleTutorialComponents> collectibleTutorialComponents;

	std::map<char, TextChar> textChars; //for initializing text glyphs from freetype

	// Render component containers
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<Background> backgrounds;
	ComponentContainer<Midground> midgrounds;
	ComponentContainer<Foreground> foregrounds;
	ComponentContainer<vec4> colours;
	ComponentContainer<PointLight> pointLights;


	// Spawnable types
	std::unordered_map<std::string, ContainerInterface*> spawnable_lists;
	ComponentContainer<Boar> boars;
	ComponentContainer<Barbarian> barbarians;
	ComponentContainer<Archer> archers;
	ComponentContainer<Bird> birds;
	ComponentContainer<Wizard> wizards;
	ComponentContainer<Troll> trolls;
	ComponentContainer<Heart> hearts;
	ComponentContainer<CollectibleTrap> collectibleTraps;

	GameTimer gameTimer;
	GameScore gameScore;

	//debugging
	FPSTracker fpsTracker;

	ECSRegistry()
	{
		registry_list.push_back(&players);
		registry_list.push_back(&dashers);
		registry_list.push_back(&enemies);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&cooldowns);
		registry_list.push_back(&collectibles);
		registry_list.push_back(&traps);
		registry_list.push_back(&damageds);
		registry_list.push_back(&damagings);
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&knockables);
		registry_list.push_back(&knockers);
		registry_list.push_back(&trappables);
		registry_list.push_back(&staminas);
		registry_list.push_back(&mapTiles);
		registry_list.push_back(&obstacles);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&collected);
		
		registry_list.push_back(&healthBars);
		registry_list.push_back(&staminaBars);
		registry_list.push_back(&texts);
		registry_list.push_back(&jumpers);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&targetAreas);

		registry_list.push_back(&renderRequests);
		registry_list.push_back(&backgrounds);
		registry_list.push_back(&midgrounds);
		registry_list.push_back(&foregrounds);
		registry_list.push_back(&colours);
		registry_list.push_back(&pointLights);

		registry_list.push_back(&boars);
		registry_list.push_back(&barbarians);
		registry_list.push_back(&archers);
		registry_list.push_back(&birds);
		registry_list.push_back(&wizards);
		registry_list.push_back(&trolls);
		registry_list.push_back(&hearts);
		registry_list.push_back(&collectibleTraps);

		registry_list.push_back(&animationControllers);

		registry_list.push_back(&pauseMenuComponents);
		registry_list.push_back(&helpMenuComponents);
		registry_list.push_back(&tutorialComponents);
		registry_list.push_back(&enemyTutorialComponents);
		registry_list.push_back(&collectibleTutorialComponents);

		spawnable_lists["boar"] = &boars;
		spawnable_lists["barbarian"] = &barbarians;
		spawnable_lists["archer"] = &archers;
		spawnable_lists["bird"] = &birds;
		spawnable_lists["wizard"] = &wizards;
		spawnable_lists["troll"] = &trolls;
		spawnable_lists["heart"] = &hearts;
		spawnable_lists["collectible_trap"] = &collectibleTraps;
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