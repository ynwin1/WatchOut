#include "sound_system.hpp"

SoundSystem::SoundSystem() : backgroundMusic(nullptr), musicTracks(), soundEffects()
{
}

SoundSystem::~SoundSystem()
{
	stopAllSounds();
	Mix_CloseAudio();
}

void handleError(const std::string& message)
{
	std::cerr << message << " SDL_mixer Error: " << Mix_GetError() << std::endl;
}

bool SoundSystem::init()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		handleError("SDL could not initialize!");
		return false;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		handleError("SDL_mixer could not initialize!");
		return false;
	}

	return true;
}

void SoundSystem::playMusic(const std::string& key, std::string path, int duration, int volume)
{
	Mix_Music* music = Mix_LoadMUS(path.c_str());
	if (music == nullptr)
	{
		handleError("Failed to load music!");
		return;
	}
	int channel = Mix_PlayMusic(music, duration);
	if (channel == -1)
	{
		handleError("Failed to play music!");
		return;
	}
	Mix_VolumeMusic(volume);
	musicTracks[key] = std::make_pair(music, channel);
}

// count - number of times to play the sound, 0 means once, -1 means infinite loop
void SoundSystem::playSoundEffect(const std::string& key, std::string path, int count)
{
	Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
	if (sound == nullptr)
	{
		handleError("Failed to load sound effect!");
		return;
	}
	int channel = Mix_PlayChannel(-1, sound, count);
	if (channel == -1)
	{
		handleError("Failed to play sound effect!");
		return;
	}
	soundEffects[key] = std::make_pair(sound, channel);
};

// stop specified music
void SoundSystem::stopMusic(const std::string& key)
{
	std::pair<Mix_Music*, int> music = musicTracks[key];
	Mix_HaltChannel(music.second);
	Mix_FreeMusic(music.first);
	musicTracks.erase(key);
}

// stop specified sound effect
void SoundSystem::stopSoundEffect(const std::string& key)
{
	std::pair<Mix_Chunk*, int> sound = soundEffects[key];
	Mix_HaltChannel(sound.second);
	Mix_FreeChunk(sound.first);
	soundEffects.erase(key);
}

// stop all music
void SoundSystem::stopAllMusic() {
	for (auto musicTrack : musicTracks) {
		std::pair<Mix_Music*, int> music = musicTrack.second;
		Mix_HaltChannel(music.second);
		Mix_FreeMusic(music.first);
	}
	musicTracks.clear();
}

// stop all sound effects
void SoundSystem::stopAllSoundEffects() {
	for (auto soundEffect : soundEffects) {
		std::pair<Mix_Chunk*, int> sound = soundEffect.second;
		Mix_HaltChannel(sound.second);
		Mix_FreeChunk(sound.first);
	}
	soundEffects.clear();
}

// stop all sounds (music and sound effects)
void SoundSystem::stopAllSounds() {
	stopAllMusic();
	stopAllSoundEffects();
}