#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

#include <Enlivengine/System/PrimitiveTypes.hpp>

#include <Enlivengine/Graphics/SFMLResources.hpp>

namespace en
{

using MusicId = U32;
using SoundId = U32;

class AudioSystem
{
    public:
        using MusicPtr = std::shared_ptr<sf::Music>;
		using MusicPtrList = std::vector<MusicPtr>;

        using SoundPtr = std::shared_ptr<sf::Sound>;
		using SoundPtrList = std::vector<SoundPtr>;

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
		MusicId PrepareMusic(const char* id, const std::string& filename);
		MusicPtr PlayMusic(MusicId id, bool loop = true);
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
		SoundId PrepareSound(const char* id, const std::string& filename);
		bool IsSoundLoaded(SoundId id) const;
		bool IsSoundLoaded(const char* id) const;
		U32 GetLoadedSoundsCount() const;
		SoundPtr PlaySound(SoundId id);
		SoundPtr PlaySound(const char* id);
		U32 GetCurrentSoundsCount() const;
		void ReleaseSound(SoundId id);
		void ReleaseSound(const char* id);
		void PlaySounds();
		void PauseSounds();
		void StopSounds();
		void ReleaseSounds();

	private:
		friend class Application;
		void Update();

		void UpdateMusicsVolume();
		void UpdateSoundsVolume();

	private:
		F32 mGlobalVolume;
		bool mGlobalEnabled;
		bool mPlaying;

		std::unordered_map<MusicId, std::string> mMusicFilenames;
		F32 mMusicsVolume;
		bool mMusicsEnabled;
		MusicPtrList mMusics;

		ResourceManager& mResourceManager;
		std::vector<SoundId> mLoadedSounds;
		F32 mSoundsVolume;
		bool mSoundsEnabled;
		SoundPtrList mSounds;

		static const U32 MAX_MUSIC = 16;
		static const U32 MAX_SOUND = 240;
};

} // namespace en
