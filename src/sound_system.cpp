#include "sound_system.hpp"

SoundSystem::SoundSystem(): backgroundMusic(nullptr) {}

SoundSystem::~SoundSystem()
{
	if (backgroundMusic != nullptr) {
		Mix_FreeMusic(backgroundMusic);
		backgroundMusic = nullptr;
	}

	Mix_CloseAudio();
}

bool SoundSystem::init()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return false;
	}

	return true;
}

void SoundSystem::playMusic(std::string path, int duration)
{
	Mix_Music* music = Mix_LoadMUS(path.c_str());
	if (music == nullptr)
	{
		std::cerr << "Failed to load sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}

	if (Mix_PlayMusic(music, duration) == -1)
	{
		std::cerr << "Failed to play sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}
	// Mix_FreeMusic(music);
}

// count - number of times to play the sound, 0 means once, -1 means infinite loop
void SoundSystem::playSoundEffect(std::string path, int count)
{
	Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
	if (sound == nullptr)
	{
		std::cerr << "Failed to load sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}

	if (Mix_PlayChannel(-1, sound, count) == -1)
	{
		std::cerr << "Failed to play sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}
	// Mix_FreeChunk(sound);
};

