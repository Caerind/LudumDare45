#include <Enlivengine/Application/AudioSystem.hpp>

namespace en
{

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

MusicId AudioSystem::PrepareMusic(const char* id, const std::string& filename)
{
	MusicId index(priv::StringToResourceID(id));
	if (mMusicFilenames.find(index) == mMusicFilenames.end())
	{
		mMusicFilenames[index] = filename;
	}
	return index;
}

AudioSystem::MusicPtr AudioSystem::PlayMusic(MusicId id, bool loop /*= true*/)
{
	if (mMusicFilenames.find(id) != mMusicFilenames.end() && mMusics.size() < MAX_MUSIC)
	{
		MusicPtr m(std::make_shared<sf::Music>());
		mMusics.push_back(m);
		m->openFromFile(mMusicFilenames[id]);
		m->setLoop(loop);
		m->setVolume(GetCurrentMusicsVolume());
		m->play();
		if (!mPlaying)
		{
			m->pause();
		}
		return m;
	}

	return nullptr;
}

AudioSystem::MusicPtr AudioSystem::PlayMusic(const char* id, bool loop /*= true*/)
{
	return PlayMusic(priv::StringToResourceID(id), loop);
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

SoundId AudioSystem::PrepareSound(const char* id, const std::string& filename)
{
	const SoundBufferPtr soundBuffer = mResourceManager.Create(id, SoundBufferLoader::FromFile(filename));
	if (soundBuffer.IsValid())
	{
		const SoundId soundId = soundBuffer.GetID();
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

bool AudioSystem::IsSoundLoaded(SoundId id) const
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

AudioSystem::SoundPtr AudioSystem::PlaySound(SoundId id)
{
	if (IsSoundLoaded(id) && mResourceManager.Has(id) && mSounds.size() < MAX_SOUND)
	{
		SoundPtr s(std::make_shared<sf::Sound>());
		mSounds.push_back(s);
		s->setBuffer(mResourceManager.Get<en::SoundBuffer>(id).Get());
		s->setVolume(GetCurrentSoundsVolume());
		s->play();
		if (!mPlaying)
		{
			s->pause();
		}
		return s;
	}

	return nullptr;
}

AudioSystem::SoundPtr AudioSystem::PlaySound(const char* id)
{
	return PlaySound(priv::StringToResourceID(id));
}

U32 AudioSystem::GetCurrentSoundsCount() const
{
	return static_cast<U32>(mLoadedSounds.size());
}

void AudioSystem::ReleaseSound(SoundId id)
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
			mSounds.erase(mSounds.begin() + i);
			soundsSize--;
		}
		else
		{
			i++;
		}
	}
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
