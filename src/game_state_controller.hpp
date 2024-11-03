enum class GAME_STATE
{
    PLAYING,
    PAUSED,
    GAMEOVER,
    HELP
};

class GameStateController
{
public:
    GameStateController(GAME_STATE initialGameState){
        currentGameState = initialGameState;
    }

    const GAME_STATE getGameState() {
        return currentGameState;
    }

    void setGameState(GAME_STATE newGameState) {
        currentGameState = newGameState;
    }
private:
    GAME_STATE currentGameState;
};