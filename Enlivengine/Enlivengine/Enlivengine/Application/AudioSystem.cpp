#include <Enlivengine/Application/AudioSystem.hpp>

namespace en
{

U32 Music::sMusicUIDGenerator = 0;
U32 Sound::sSoundUIDGenerator = 0;

Music::Music(MusicID musicID, const std::string& filename)
{
	if (musicID != InvalidMusicID && openFromFile(filename))
	{
		mMusicID = musicID;
		mMusicUID = sMusicUIDGenerator++;
	}
	else
	{
		mMusicID = InvalidMusicID;
		mMusicUID = InvalidMusicUID;
	}
}

bool Music::IsValid() const
{
	return mMusicID != InvalidMusicID && mMusicUID != InvalidMusicUID;
}

MusicID Music::GetMusicID() const
{
	return mMusicID;
}

U32 Music::GetUID() const
{
	return mMusicUID;
}

MusicPtr::MusicPtr(AudioSystem* audioSystem, MusicID musicID, U32 musicUID)
	: mAudioSystem(audioSystem)
	, mMusicID(musicID)
	, mMusicUID(musicUID)
{
}

bool MusicPtr::IsValid() const
{
	return GetMusic() != nullptr;
}

MusicID MusicPtr::GetMusicID() const
{
	return mMusicID;
}

U32 MusicPtr::GetUID() const
{
	return mMusicUID;
}

en::Music* MusicPtr::GetMusic()
{
	if (mAudioSystem != nullptr && mMusicID != InvalidMusicID && mMusicUID != InvalidMusicUID)
	{
		return mAudioSystem->GetMusicInternal(mMusicUID);
	}
	return nullptr;
}

const en::Music* MusicPtr::GetMusic() const
{
	if (mAudioSystem != nullptr && mMusicID != InvalidMusicID && mMusicUID != InvalidMusicUID)
	{
		return mAudioSystem->GetMusicInternal(mMusicUID);
	}
	return nullptr;
}

Sound::Sound(SoundBufferPtr soundBuffer)
{
	if (soundBuffer.IsValid())
	{
		setBuffer(soundBuffer.Get());
		mSoundID = static_cast<SoundID>(soundBuffer.GetID());
		mSoundUID = sSoundUIDGenerator++;
	}
	else
	{
		mSoundID = InvalidSoundID;
		mSoundUID = InvalidSoundUID;
	}
}

bool Sound::IsValid() const
{
	return mSoundID != InvalidSoundID && mSoundUID != InvalidSoundUID;
}

SoundID Sound::GetSoundID() const
{
	return mSoundID;
}

U32 Sound::GetUID() const
{
	return mSoundUID;
}

SoundPtr::SoundPtr(AudioSystem* audioSystem, SoundID soundID, U32 soundUID)
	: mAudioSystem(audioSystem)
	, mSoundID(soundID)
	, mSoundUID(soundUID)
{
}

bool SoundPtr::IsValid() const
{
	return GetSound() != nullptr;
}

SoundID SoundPtr::GetSoundID() const
{
	return mSoundID;
}

U32 SoundPtr::GetUID() const
{
	return mSoundUID;
}

en::Sound* SoundPtr::GetSound()
{
	if (mAudioSystem != nullptr && mSoundID != InvalidSoundID && mSoundUID != InvalidSoundUID)
	{
		return mAudioSystem->GetSoundInternal(mSoundUID);
	}
	return nullptr;
}

const en::Sound* SoundPtr::GetSound() const
{
	if (mAudioSystem != nullptr && mSoundID != InvalidSoundID && mSoundUID != InvalidSoundUID)
	{
		return mAudioSystem->GetSoundInternal(mSoundUID);
	}
	return nullptr;
}

AudioSystem::AudioSystem(ResourceManager& resourceManager)
	: mGlobalVolume(100.0f)
	, mGlobalEnabled(true)
	, mPlaying(true)
	, mMusicFilenames()
	, mMusicsVolume(100.0f)
	, mMusicsEnabled(true)
	, mMusics()
	, mResourceManager(resourceManager)
	, mLoadedSounds()
	, mSoundsVolume(100.0f)
	, mSoundsEnabled(true)
	, mSounds()
{
}

F32 AudioSystem::GetGlobalVolume() const
{
	return mGlobalVolume;
}

void AudioSystem::SetGlobalVolume(F32 volume)
{
	mGlobalVolume = volume;
	sf::Listener::setGlobalVolume(volume);
}

bool AudioSystem::IsEnabled() const
{
	return mGlobalEnabled;
}

void AudioSystem::SetEnabled(bool enabled)
{
	mGlobalEnabled = enabled;
	UpdateMusicsVolume();
	UpdateSoundsVolume();
}

void AudioSystem::Play()
{
	mPlaying = true;
	PlayMusics();
	PlaySounds();
}

void AudioSystem::Pause()
{
	mPlaying = false;
	PauseMusics();
	PauseSounds();
}

void AudioSystem::Stop()
{
	StopMusics();
	StopSounds();
}

bool AudioSystem::IsPlaying() const
{
	return mPlaying;
}

bool AudioSystem::IsInactive() const
{
	return (GetCurrentMusicsCount() + GetCurrentSoundsCount()) == 0;
}

void AudioSystem::Clear()
{
	ReleaseSounds();
}

F32 AudioSystem::GetMusicVolume() const
{
	return mMusicsVolume;
}

void AudioSystem::SetMusicVolume(F32 volume)
{
	mMusicsVolume = volume;
	UpdateMusicsVolume();
}

bool AudioSystem::AreMusicsEnabled() const
{
	return mMusicsEnabled;
}

void AudioSystem::SetMusicsEnabled(bool enabled)
{
	mMusicsEnabled = enabled;
	UpdateMusicsVolume();
}

F32 AudioSystem::GetCurrentMusicsVolume() const
{
	if (!mGlobalEnabled || !mMusicsEnabled)
	{
		return 0.0f;
	}
	else
	{
		return mMusicsVolume;
	}
}

MusicID AudioSystem::PrepareMusic(const char* id, const std::string& filename)
{
	MusicID index(priv::StringToResourceID(id));
	if (mMusicFilenames.find(index) == mMusicFilenames.end())
	{
		mMusicFilenames[index] = filename;
	}
	return index;
}

MusicPtr AudioSystem::PlayMusic(MusicID id, bool loop /*= true*/)
{
	const auto itr = mMusicFilenames.find(id);
	if (mMusics.size() < MAX_MUSICS && itr != mMusicFilenames.end())
	{
		mMusics.push_back(new Music(id, itr->second));
		Music* music = mMusics.back();
		if (music != nullptr && music->IsValid())
		{
			music->setLoop(loop);
			music->setVolume(GetCurrentMusicsVolume());
			music->play();
			if (!mPlaying)
			{
				music->pause();
			}
			return std::move(MusicPtr(this, music->GetMusicID(), music->GetUID()));
		}
	}
	return std::move(MusicPtr());
}

MusicPtr AudioSystem::PlayMusic(const char* id, bool loop /*= true*/)
{
	return std::move(PlayMusic(priv::StringToResourceID(id), loop));
}

U32 AudioSystem::GetCurrentMusicsCount() const
{
	return static_cast<U32>(mMusics.size());
}

void AudioSystem::PlayMusics()
{
	const size_t size = mMusics.size();
	for (size_t i = 0; i < size; ++i)
	{
		mMusics[i]->play();
	}
}

void AudioSystem::PauseMusics()
{
	const size_t size = mMusics.size();
	for (size_t i = 0; i < size; ++i)
	{
		mMusics[i]->pause();
	}
}

void AudioSystem::StopMusics()
{
	const size_t size = mMusics.size();
	for (size_t i = 0; i < size; ++i)
	{
		delete mMusics[i];
		mMusics[i] = nullptr;
	}
	mMusics.clear();
}

F32 AudioSystem::GetSoundVolume() const
{
	return mSoundsVolume;
}

void AudioSystem::SetSoundVolume(F32 volume)
{
	mSoundsVolume = volume;
	UpdateSoundsVolume();
}

bool AudioSystem::AreSoundsEnabled() const
{
	return mSoundsEnabled;
}

void AudioSystem::SetSoundsEnabled(bool enabled)
{
	mSoundsEnabled = enabled;
	UpdateSoundsVolume();
}

F32 AudioSystem::GetCurrentSoundsVolume() const
{
	if (!mGlobalEnabled || !mSoundsEnabled)
	{
		return 0.0f;
	}
	else
	{
		return mSoundsVolume;
	}
}

SoundID AudioSystem::PrepareSound(const char* id, const std::string& filename)
{
	const SoundBufferPtr soundBuffer = mResourceManager.Create(id, SoundBufferLoader::FromFile(filename));
	if (soundBuffer.IsValid())
	{
		const SoundID soundId = soundBuffer.GetID();
		if (!IsSoundLoaded(soundId))
		{
			mLoadedSounds.push_back(soundId);
		}
		return soundId;
	}
	else
	{
		return InvalidResourceID;
	}
}

bool AudioSystem::IsSoundLoaded(SoundID id) const
{
	const size_t size = mLoadedSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (mLoadedSounds[i] == id)
		{
			return true;
		}
	}
	return false;
}

bool AudioSystem::IsSoundLoaded(const char* id) const
{
	return IsSoundLoaded(priv::StringToResourceID(id));
}

U32 AudioSystem::GetLoadedSoundsCount() const
{
	return static_cast<U32>(mLoadedSounds.size());
}

SoundPtr AudioSystem::PlaySound(SoundID id)
{
	if (mSounds.size() < MAX_SOUNDS && IsSoundLoaded(id) && mResourceManager.Has(id))
	{
		const SoundBufferPtr soundBuffer = mResourceManager.Get<en::SoundBuffer>(id);
		if (soundBuffer.IsValid())
		{
			mSounds.push_back(new Sound(soundBuffer));
			Sound* sound = mSounds.back();
			if (sound != nullptr && sound->IsValid())
			{
				sound->setVolume(GetCurrentSoundsVolume());
				sound->play();
				if (!mPlaying)
				{
					sound->pause();
				}
				return std::move(SoundPtr(this, sound->GetSoundID(), sound->GetUID()));
			}
		}
	}
	return std::move(SoundPtr());
}

SoundPtr AudioSystem::PlaySound(const char* id)
{
	return std::move(PlaySound(priv::StringToResourceID(id)));
}

U32 AudioSystem::GetCurrentSoundsCount() const
{
	return static_cast<U32>(mLoadedSounds.size());
}

void AudioSystem::ReleaseSound(SoundID id)
{
	const size_t size = mLoadedSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (mLoadedSounds[i] == id)
		{
			mResourceManager.Release(id);
			mLoadedSounds.erase(mLoadedSounds.begin() + i);
			return;
		}
	}
}

void AudioSystem::ReleaseSound(const char* id)
{
	ReleaseSound(priv::StringToResourceID(id));
}

void AudioSystem::PlaySounds()
{
	const size_t size = mSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		mSounds[i]->play();
	}
}

void AudioSystem::PauseSounds()
{
	const size_t size = mSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		mSounds[i]->pause();
	}
}

void AudioSystem::StopSounds()
{
	const size_t size = mSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		delete mSounds[i];
		mSounds[i] = nullptr;
	}
	mSounds.clear();
}

void AudioSystem::ReleaseSounds()
{
	const size_t size = mLoadedSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		mResourceManager.Release(mLoadedSounds[i]);
	}
	mLoadedSounds.clear();
}

void AudioSystem::Update()
{
	if (!mPlaying)
		return;

	size_t musicsSize = mMusics.size();
	for (size_t i = 0; i < musicsSize; )
	{
		if (mMusics[i]->getStatus() == sf::SoundSource::Stopped)
		{
			delete mMusics[i];
			mMusics[i] = nullptr;
			mMusics.erase(mMusics.begin() + i);
			musicsSize--;
		}
		else
		{
			i++;
		}
	}

	size_t soundsSize = mSounds.size();
	for (size_t i = 0; i < soundsSize; )
	{
		if (mSounds[i]->getStatus() == sf::SoundSource::Stopped)
		{
			delete mSounds[i];
			mSounds[i] = nullptr;
			mSounds.erase(mSounds.begin() + i);
			soundsSize--;
		}
		else
		{
			i++;
		}
	}
}

en::Music* AudioSystem::GetMusicInternal(U32 musicUID)
{
	const size_t size = mMusics.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (mMusics[i]->GetUID() == musicUID)
		{
			return mMusics[i];
		}
	}
	return nullptr;
}

const en::Music* AudioSystem::GetMusicInternal(U32 musicUID) const
{
	const size_t size = mMusics.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (mMusics[i]->GetUID() == musicUID)
		{
			return mMusics[i];
		}
	}
	return nullptr;
}

en::Sound* AudioSystem::GetSoundInternal(U32 soundUID)
{
	const size_t size = mSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (mSounds[i]->GetUID() == soundUID)
		{
			return mSounds[i];
		}
	}
	return nullptr;
}

const en::Sound* AudioSystem::GetSoundInternal(U32 soundUID) const
{
	const size_t size = mSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (mSounds[i]->GetUID() == soundUID)
		{
			return mSounds[i];
		}
	}
	return nullptr;
}

void AudioSystem::UpdateMusicsVolume()
{
	const F32 currentVolume = GetCurrentMusicsVolume();
	const size_t size = mMusics.size();
	for (size_t i = 0; i < size; ++i)
	{
		mMusics[i]->setVolume(currentVolume);
	}
}

void AudioSystem::UpdateSoundsVolume()
{
	const F32 currentVolume = GetCurrentSoundsVolume();
	const size_t size = mSounds.size();
	for (size_t i = 0; i < size; ++i)
	{
		mSounds[i]->setVolume(currentVolume);
	}
}

} // namespace en
