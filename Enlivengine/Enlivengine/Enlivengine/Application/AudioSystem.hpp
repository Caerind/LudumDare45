#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

#include <Enlivengine/System/PrimitiveTypes.hpp>

#include <Enlivengine/Graphics/SFMLResources.hpp>

namespace en
{

class AudioSystem;

using MusicID = U32;
constexpr MusicID InvalidMusicID = U32_Max;
constexpr U32 InvalidMusicUID = U32_Max;

// It's recommend to not use Music directly
// Use the AudioSystem + MusicPtr
class Music : public sf::Music
{
    public:
	    Music(MusicID musicID, const std::string& filename);

		bool IsValid() const;
		MusicID GetMusicID() const;
		U32 GetUID() const;

	private:
		MusicID mMusicID;
		U32 mMusicUID;

		static U32 sMusicUIDGenerator;
};

// This class is a Ptr to a music managed by the AudioSystem
class MusicPtr
{
    public:
	    MusicPtr(AudioSystem* audioSystem = nullptr, MusicID musicID = InvalidMusicID, U32 musicUID = InvalidMusicUID);

		bool IsValid() const;
		MusicID GetMusicID() const;
		U32 GetUID() const;

    private:
		Music* GetMusic();
		const Music* GetMusic() const;

    private:
		AudioSystem* mAudioSystem;
		MusicID mMusicID;
		U32 mMusicUID;
};

using SoundID = U32;
constexpr SoundID InvalidSoundID = U32_Max;
constexpr U32 InvalidSoundUID = U32_Max;

// It's recommend to not use Sound directly
// Use the AudioSystem + SoundPtr
class Sound : public sf::Sound
{
    public:
		Sound(SoundBufferPtr soundBuffer);

		bool IsValid() const;
		SoundID GetSoundID() const;
		U32 GetUID() const;

    private:
	    SoundID mSoundID;
		U32 mSoundUID;

		static U32 sSoundUIDGenerator;
};

// This class is a Ptr to a sound managed by the AudioSystem
class SoundPtr
{
    public:
	    SoundPtr(AudioSystem* audioSystem = nullptr, SoundID soundID = InvalidSoundID, U32 soundUID = InvalidSoundUID);

		bool IsValid() const;
		SoundID GetSoundID() const;
		U32 GetUID() const;

    private:
		Sound* GetSound();
		const Sound* GetSound() const;

    private:
		AudioSystem* mAudioSystem;
		SoundID mSoundID;
		U32 mSoundUID;
};

// Class that smartly manage audio sources
class AudioSystem
{
    public:
        AudioSystem(ResourceManager& resourceManager);

		F32 GetGlobalVolume() const;
		void SetGlobalVolume(F32 volume);
		bool IsEnabled() const;
		void SetEnabled(bool enabled);
		void Play();
		void Pause();
		void Stop(); // Stop everything, but keep the same play/pause state
		bool IsPlaying() const;
		bool IsInactive() const; // No sounds and no musics
		void Clear();

		F32 GetMusicVolume() const;
		void SetMusicVolume(F32 volume);
		bool AreMusicsEnabled() const;
		void SetMusicsEnabled(bool enabled);
		F32 GetCurrentMusicsVolume() const;
		MusicID PrepareMusic(const char* id, const std::string& filename);
		MusicPtr PlayMusic(MusicID id, bool loop = true);
		MusicPtr PlayMusic(const char* id, bool loop = true);
		U32 GetCurrentMusicsCount() const;
		void PlayMusics();
		void PauseMusics();
		void StopMusics();

		F32 GetSoundVolume() const;
		void SetSoundVolume(F32 volume);
		bool AreSoundsEnabled() const;
		void SetSoundsEnabled(bool enabled);
		F32 GetCurrentSoundsVolume() const;
		SoundID PrepareSound(const char* id, const std::string& filename);
		bool IsSoundLoaded(SoundID id) const;
		bool IsSoundLoaded(const char* id) const;
		U32 GetLoadedSoundsCount() const;
		SoundPtr PlaySound(SoundID id);
		SoundPtr PlaySound(const char* id);
		U32 GetCurrentSoundsCount() const;
		void ReleaseSound(SoundID id);
		void ReleaseSound(const char* id);
		void PlaySounds();
		void PauseSounds();
		void StopSounds();
		void ReleaseSounds();

	private:
		friend class Application;
		void Update();

		friend class MusicPtr;
		en::Music* GetMusicInternal(U32 musicUID);
		const en::Music* GetMusicInternal(U32 musicUID) const;

		friend class SoundPtr;
		en::Sound* GetSoundInternal(U32 soundUID);
		const en::Sound* GetSoundInternal(U32 soundUID) const;

		void UpdateMusicsVolume();
		void UpdateSoundsVolume();

	private:
		F32 mGlobalVolume;
		bool mGlobalEnabled;
		bool mPlaying;

		std::unordered_map<MusicID, std::string> mMusicFilenames;
		F32 mMusicsVolume;
		bool mMusicsEnabled;
		std::vector<Music*> mMusics;

		ResourceManager& mResourceManager;
		std::vector<SoundID> mLoadedSounds;
		F32 mSoundsVolume;
		bool mSoundsEnabled;
		std::vector<Sound*> mSounds;

		static constexpr U32 MAX_MUSICS = 16;
		static constexpr U32 MAX_SOUNDS = 240;
};

} // namespace en
