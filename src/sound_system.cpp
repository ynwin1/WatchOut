#include "sound_system.hpp"
#include "tiny_ecs_registry.hpp"
#include <chrono>

SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem()
{
	stopAllSounds();
	unloadAllSounds();
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

	int numChannels = 32;
	Mix_AllocateChannels(numChannels);

	loadAllMusic();
	loadAllSoundEffects();

	return true;
}

void SoundSystem::loadAllMusic()
{
	for (auto& pair : musics) {
		Mix_Music* music = Mix_LoadMUS(pair.second.first.c_str());
		if (music == nullptr) {
			handleError("Failed to load music!");
			continue;
		}
		loadedMusic[pair.first] = music;
	}
}

void SoundSystem::loadAllSoundEffects()
{
	for (auto& pair : sounds) {
		Mix_Chunk* sound = Mix_LoadWAV(pair.second.first.c_str());
		if (sound == nullptr) {
			handleError("Failed to load sound effect!");
			continue;
		}
		loadedSoundEffects[pair.first] = sound;
	}
}

void SoundSystem::playMusic(Music key, int duration)
{
	auto it = loadedMusic.find(key);
	if (it == loadedMusic.end()) {
		printf("Failed to find loaded music %d\n", key);
		return;
	}
	Mix_Music* music = it->second;
	int channel = Mix_PlayMusic(music, duration);
	if (channel == -1) {
		handleError("Failed to play music!");
		return;
	}
	int originalVolume = musics.find(key)->second.second;
	if (mute) {
		Mix_VolumeMusic(0);
	}
	else {
		Mix_VolumeMusic(originalVolume);
	}
	musicTracks[key] = channel;
}

// count - number of times to play the sound, 0 means once, -1 means infinite loop
void SoundSystem::playSoundEffect(Sound key, int count)
{
	auto it = loadedSoundEffects.find(key);
	if (it == loadedSoundEffects.end()) {
		printf("Failed to find loaded sound effect %d\n", key);
		return;
	}
	Mix_Chunk* sound = it->second;
	int channel = Mix_PlayChannel(-1, sound, count);
	if (channel == -1) {
		handleError("Failed to play sound effect!");
		return;
	}
	int originalVolume = sounds.find(key)->second.second;
	if (mute) {
		Mix_Volume(channel, 0);
	}
	else {
		Mix_Volume(channel, originalVolume);
	}
	soundEffects[key] = channel;
};

// pause specified music
void SoundSystem::pauseMusic(Music key)
{
	if (musicTracks.find(key) == musicTracks.end())
	{
		handleError("Music not found!");
		return;
	}
	int channel = musicTracks[key];
	Mix_Pause(channel);
}

// pause specified sound effect
void SoundSystem::pauseSoundEffect(Sound key)
{
	if (soundEffects.find(key) == soundEffects.end())
	{
		handleError("Sound effect not found!");
		return;
	}
	int channel = soundEffects[key];
	Mix_Pause(channel);
}

// pause all music
void SoundSystem::pauseAllMusic() {
	for (auto& musicTrack : musicTracks) {
		int channel = musicTrack.second;
		Mix_Pause(channel);
	}
}

// pause all sound effects
void SoundSystem::pauseAllSoundEffects() {
	for (auto& soundEffect : soundEffects) {
		int channel = soundEffect.second;
		Mix_Pause(channel);
	}
}

// pause all sounds
void SoundSystem::pauseAllSounds() {
	pauseAllMusic();
	pauseAllSoundEffects();
}

// resume specified music
void SoundSystem::resumeMusic(Music key)
{
	if (musicTracks.find(key) == musicTracks.end())
	{
		handleError("Music not found!");
		return;
	}
	int channel = musicTracks[key];
	Mix_Resume(channel);
}

// resume specified sound effect
void SoundSystem::resumeSoundEffect(Sound key)
{
	if (soundEffects.find(key) == soundEffects.end())
	{
		handleError("Sound effect not found!");
		return;
	}
	int channel = soundEffects[key];
	Mix_Resume(channel);
}

// resume all music
void SoundSystem::resumeAllMusic() {
	for (auto& musicTrack : musicTracks) {
		int channel = musicTrack.second;
		Mix_Resume(channel);
	}
}

// resume all sound effects
void SoundSystem::resumeAllSoundEffects() {
	for (auto& soundEffect : soundEffects) {
		int channel = soundEffect.second;
		Mix_Resume(channel);
	}
}

// resume all sounds
void SoundSystem::resumeAllSounds() {
	resumeAllMusic();
	resumeAllSoundEffects();
}

// stop specified music
void SoundSystem::stopMusic(Music key)
{
	if (musicTracks.find(key) == musicTracks.end())
	{
		handleError("Music not found!");
		return;
	}
	int channel = musicTracks[key];
	Mix_HaltChannel(channel);
	musicTracks.erase(key);
}

// stop specified sound effect
void SoundSystem::stopSoundEffect(Sound key)
{
	if (soundEffects.find(key) == soundEffects.end())
	{
		handleError("Sound effect not found!");
		return;
	}
	int channel = soundEffects[key];
	Mix_HaltChannel(channel);
	soundEffects.erase(key);
}

// stop all music
void SoundSystem::stopAllMusic() {
	for (auto& musicTrack : musicTracks) {
		int channel = musicTrack.second;
		Mix_HaltChannel(channel);
	}
	musicTracks.clear();
}

// stop all sound effects
void SoundSystem::stopAllSoundEffects() {
	for (auto& soundEffect : soundEffects) {
		int channel = soundEffect.second;
		Mix_HaltChannel(channel);
	}
	soundEffects.clear();
}

// stop all sounds (music and sound effects)
void SoundSystem::stopAllSounds() {
	stopAllMusic();
	stopAllSoundEffects();
}

// mute
void SoundSystem::muteAllSounds() {
	Mix_VolumeMusic(0);
	Mix_Volume(-1, 0);
}

// unmute
void SoundSystem::unmuteAllSounds() {
	// music (cannot control individual music volume)
	Mix_VolumeMusic(INITIAL_MUSIC_VOLUME);
	// sound effects
	for (auto& soundEffect : soundEffects) {
		int originalVolume = sounds.find(soundEffect.first)->second.second;
		Mix_Volume(soundEffect.second, originalVolume);
	}
}

void SoundSystem::unloadAllSounds()
{
	for (auto& pair : loadedMusic) {
		Mix_FreeMusic(pair.second);
	}
	loadedMusic.clear();

	for (auto& pair : loadedSoundEffects) {
		Mix_FreeChunk(pair.second);
	}
	loadedSoundEffects.clear();
}

void SoundSystem::controlPlayerSound() {
	Player& player = registry.players.components[0];
	if (player.isMoving) {
		if (!isMovingSoundPlaying) {
			// walking sound
			playSoundEffect(Sound::WALKING, -1);
			isMovingSoundPlaying = true;
			printf("isMovingSoundPlaying = true\n");
		}
	}
	else {
		if (isMovingSoundPlaying) {
			// stop walking sound
			printf("isMovingSoundPlaying = false\n");
			stopSoundEffect(Sound::WALKING);
			isMovingSoundPlaying = false;
		}
	}
}

void SoundSystem::controlBirdSound() {
	// monitoring birds movement
	if (registry.birds.size() > 0) {
		if (!isBirdFlockSoundPlaying) {
			// birds sound
			playSoundEffect(Sound::BIRD_FLOCK, -1);
			isBirdFlockSoundPlaying = true;
		}
	}
	else {
		if (isBirdFlockSoundPlaying) {
			// stop birds sound
			stopSoundEffect(Sound::BIRD_FLOCK);
			isBirdFlockSoundPlaying = false;
		}
	}
}

void SoundSystem::step(float elapsed_ms)
{
	controlPlayerSound();
	controlBirdSound();

	// troll laughs
	for (auto& troll : registry.trolls.components) {
		troll.laughCooldown -= elapsed_ms;
		if (troll.laughCooldown <= 0.f) {
			playSoundEffect(Sound::TROLL_LAUGH, 0);
			troll.laughCooldown = TROLL_LAUGH_COOLDOWN;
		}
	}
}
