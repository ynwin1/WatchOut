#pragma once
// Sound system libraries
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <map>
#include <string>

#include "common.hpp"

enum class Music {
	BACKGROUND,
	PLAYER_DEATH
};

enum class Sound {
	ARROW,
	BOAR_CHARGE,
	THUNDER,
	STORM,
	WALKING,
	JUMPING,
	DASHING,
	LEVELUP,
	FIREBALL,
	COLLECT,
	BIRD_FLOCK,
	BIRD_ATTACK
};

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	bool mute = false;
	int INITIAL_MUSIC_VOLUME = 20;

	// path and volume
	const std::map<Music, std::pair<std::string, int>> musics = {
		{ Music::BACKGROUND, std::pair<std::string, int>(audio_path("mystery_background.wav"), INITIAL_MUSIC_VOLUME) },
		{ Music::PLAYER_DEATH,	std::pair<std::string, int>(audio_path("playerDeath.wav"), INITIAL_MUSIC_VOLUME) }
	};

	const std::map<Sound, std::pair<std::string, int>> sounds = {
		{ Sound::ARROW,			std::pair<std::string, int>(audio_path("arrow.wav"), 80) },
		{ Sound::BOAR_CHARGE,	std::pair<std::string, int>(audio_path("boar_charge.wav"), 80) },
		{ Sound::THUNDER,		std::pair<std::string, int>(audio_path("thunder.wav"), 80) },
		{ Sound::STORM,			std::pair<std::string, int>(audio_path("storm.wav"), 80) },
		{ Sound::WALKING,		std::pair<std::string, int>(audio_path("walking.wav"), 80) },
		{ Sound::JUMPING,		std::pair<std::string, int>(audio_path("jumping.wav"), 80) },
		{ Sound::DASHING,		std::pair<std::string, int>(audio_path("dashing.wav"), 80) },
		{ Sound::LEVELUP,		std::pair<std::string, int>(audio_path("levelUp.wav"), 80) },
		{ Sound::FIREBALL,		std::pair<std::string, int>(audio_path("fireball.wav"), 80) },
		{ Sound::COLLECT,		std::pair<std::string, int>(audio_path("collect.wav"),80) },
		{ Sound::BIRD_FLOCK,	std::pair<std::string, int>(audio_path("birds_flock.wav"), 50) },
		{ Sound::BIRD_ATTACK,	std::pair<std::string, int>(audio_path("bird_attack.wav"), 80) }
	};

	// Sound variables
	bool isMovingSoundPlaying = false;
	bool isBirdFlockSoundPlaying = false;

	// FUNCTIONS
	// Initialize sound system
	bool init();

	void loadAllMusic();
	void loadAllSoundEffects();

	void step(float elapsed_ms);

	// PLAY
	// Play music; duration = -1 for infinite loop
	void playMusic(Music key, int duration, int volume);
	// Play sound effect
	void playSoundEffect(Sound key, int count, int volume);

	// PAUSE
	// Pause music
	void pauseMusic(Music key);
	// Pause sound effect
	void pauseSoundEffect(Sound key);
	// Pause all music
	void pauseAllMusic();
	// Pause all sound effects
	void pauseAllSoundEffects();
	// Pause all sounds
	void pauseAllSounds();

	// RESUME
	// Resume music
	void resumeMusic(Music key);
	// Resume sound effect
	void resumeSoundEffect(Sound key);
	// Resume all music
	void resumeAllMusic();
	// Resume all sound effects
	void resumeAllSoundEffects();
	// Resume all sounds
	void resumeAllSounds();

	// STOP
	// Stop music
	void stopMusic(Music key);
	// Stop sound effect
	void stopSoundEffect(Sound key);
	// Stop all music
	void stopAllMusic();
	// Stop all sound effects
	void stopAllSoundEffects();
	// Stop all sounds
	void stopAllSounds();

	// MUTE
	// Mute all sounds
	void muteAllSounds();
	// Unmute all sounds
	void unmuteAllSounds();

	// Remove all sounds from memory
	void unloadAllSounds();

	// Special functions
	void controlPlayerSound();
	void controlBirdSound();

private:
	std::map<Music, Mix_Music*> loadedMusic;
	std::map<Sound, Mix_Chunk*> loadedSoundEffects;
	std::map<Music, int> musicTracks;
	std::map<Sound, int> soundEffects;
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
- Level Up: https://freesound.org/people/gsb1039/sounds/415061/
- Fireball: https://freesound.org/people/LiamG_SFX/sounds/334235/
- Collect: https://freesound.org/people/METKIR/sounds/623593/
- Bird Flock: https://www.youtube.com/watch?v=22dZEAMmxpk
- Bird Attack: https://freesound.org/people/dinodilopho/sounds/263530/
*/