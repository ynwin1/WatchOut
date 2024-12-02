#include "game_state_controller.hpp"

#include "world_system.hpp"
#include "world_init.hpp"
#include "common.hpp"

void GameStateController::init(GAME_STATE initialGameState, WorldSystem* w) {
    currentGameState = initialGameState;
    world = w;
}

const GAME_STATE GameStateController::getGameState() const {
    return currentGameState;
}

void GameStateController::setGameState(GAME_STATE newGameState) {
    onExitState(currentGameState); // Call onExitState before changing the state
    beforeEnterState(newGameState);
    currentGameState = newGameState;
}

void GameStateController::onExitState(GAME_STATE oldState) {
    // Implement the logic for exiting a state
    if (oldState == GAME_STATE::TUTORIAL) {
        world->exitTutorial();
    }
    if (oldState == GAME_STATE::BOAR_TUTORIAL || oldState == GAME_STATE::WIZARD_TUTORIAL ||
        oldState == GAME_STATE::BIRD_TUTORIAL || oldState == GAME_STATE::TROLL_TUTORIAL ||
        oldState == GAME_STATE::ARCHER_TUTORIAL || oldState == GAME_STATE::BARBARIAN_TUTORIAL ||
        oldState == GAME_STATE::BOMBER_TUTORIAL) {
        world->exitEnemyTutorial();
    }
    if (oldState == GAME_STATE::HEART_TUTORIAL || oldState == GAME_STATE::TRAP_TUTORIAL || oldState == GAME_STATE::PHANTOM_TRAP_TUTORIAL) {
        world->exitCollectibleTutorial();
    }
    if (oldState == GAME_STATE::PAUSED) {
        world->exitPauseMenu();
    }
    if (oldState == GAME_STATE::HELP) {
        world->exitHelpMenu();
    }
}

void GameStateController::beforeEnterState(GAME_STATE newState) {
    // Implement the logic before entering a state
    if (newState == GAME_STATE::TUTORIAL) {
        world->createTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::PAUSED) {
       world->createPauseMenu(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::GAMEOVER) {
        createGameOverText(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::HELP) {
        world->createHelpMenu(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::BOAR_TUTORIAL) {
        world->createBoarTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::BIRD_TUTORIAL) {
        world->createBirdTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::WIZARD_TUTORIAL) {
        world->createWizardTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::TROLL_TUTORIAL) {
        world->createTrollTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::ARCHER_TUTORIAL) {
        world->createArcherTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::BARBARIAN_TUTORIAL) {
        world->createBarbarianTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::BOMBER_TUTORIAL) {
        world->createBomberTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::HEART_TUTORIAL) {
        world->createHeartTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::TRAP_TUTORIAL) {
        world->createTrapTutorial(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::PHANTOM_TRAP_TUTORIAL) {
        world->createPhantomTrapTutorial(world->camera->getSize());
        return;
    }
    return;
}