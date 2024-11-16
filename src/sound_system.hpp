#pragma once
// Sound system libraries
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <map>
#include <string>

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	// KEYS
	// Music
	std::string BACKGROUND_MUSIC = "background_music";
	std::string PLAYER_DEATH_MUSIC = "player_death_music";

	// Sound effects
	std::string ARROW_SOUND = "arrow_sound";
	std::string BOAR_CHARGE_SOUND = "boar_charge_sound";
	std::string THUNDER_SOUND = "thunder_sound";
	std::string STORM_SOUND = "storm_sound";
	std::string WALKING_SOUND = "walking_sound";


	// FUNCTIONS
	// Initialize sound system
	bool init();

	// Play music; duration = -1 for infinite loop
	void playMusic(const std::string& key, std::string path, int duration, int volume);

	// Play sound effect
	void playSoundEffect(const std::string& key, std::string path, int count);

	// Stop music
	void stopMusic(const std::string& key);

	// Stop sound effect
	void stopSoundEffect(const std::string& key);

	// Stop all music
	void stopAllMusic();

	// Stop all sound effects
	void stopAllSoundEffects();

	// Stop all sounds
	void stopAllSounds();

private:
	std::map<std::string, std::pair<Mix_Music*, int>> musicTracks;
	std::map<std::string, std::pair<Mix_Chunk*, int>> soundEffects;
	Mix_Music* backgroundMusic;
};

/*
Sound credits
- Background music: https://freesound.org/people/szegvari/sounds/580453/
- Mystery background: https://freesound.org/people/danlucaz/sounds/665215/
- Arrow: https://freesound.org/people/Erdie/sounds/65733/
- Boar charge: https://freesound.org/people/Robinhood76/sounds/76796/
- Thunder: https://freesound.org/people/Robinhood76/sounds/316850/
- Storm: https://pixabay.com/sound-effects/cyprus-storm-61421/
- Player Death: https://freesound.org/people/dobroide/sounds/44709/
*/