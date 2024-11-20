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

	const std::map<Music, std::string> musics = {
		{ Music::BACKGROUND,	audio_path("mystery_background.wav") },
		{ Music::PLAYER_DEATH,	audio_path("playerDeath.wav") }
	};

	const std::map<Sound, std::string> sounds = {
		{ Sound::ARROW,			audio_path("arrow.wav") },
		{ Sound::BOAR_CHARGE,	audio_path("boar_charge.wav") },
		{ Sound::THUNDER,		audio_path("thunder.wav") },
		{ Sound::STORM,			audio_path("storm.wav") },
		{ Sound::WALKING,		audio_path("walking.wav") },
		{ Sound::JUMPING,		audio_path("jumping.wav") },
		{ Sound::DASHING,		audio_path("dashing.wav") },
		{ Sound::LEVELUP,		audio_path("levelUp.wav") },
		{ Sound::FIREBALL,		audio_path("fireball.wav") },
		{ Sound::COLLECT,		audio_path("collect.wav") },
		{ Sound::BIRD_FLOCK,	audio_path("birds_flock.wav") },
		{ Sound::BIRD_ATTACK,	audio_path("bird_attack.wav") }
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

	// Play music; duration = -1 for infinite loop
	void playMusic(Music key, int duration, int volume);

	// Play sound effect
	void playSoundEffect(Sound key, int count, int volume);

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