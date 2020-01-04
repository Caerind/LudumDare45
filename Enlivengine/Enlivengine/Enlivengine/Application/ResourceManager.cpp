#include <Enlivengine/Application/ResourceManager.hpp>

namespace en
{

namespace priv
{

ResourceID StringToResourceID(const std::string& str)
{
	return Hash::CRC32(str.c_str());
}

BaseResource::BaseResource()
	: mID(InvalidResourceID)
	, mLoaded(false)
{
}

bool BaseResource::IsLoaded() const
{
	return mLoaded;
}

bool BaseResource::IsManaged() const
{
	return mID != InvalidResourceID;
}

ResourceID BaseResource::GetID() const
{
	return mID;
}

} // namespace priv

ResourceManager::ResourceManager()
{
}

bool ResourceManager::Has(const std::string& str) const
{
	return Has(priv::StringToResourceID(str));
}

bool ResourceManager::Has(ResourceID id) const
{
	return mResources.find(id) != mResources.end();
}

void ResourceManager::Release(const std::string& str)
{
	Release(priv::StringToResourceID(str));
}

void ResourceManager::Release(ResourceID id)
{
	const auto itr = mResources.find(id);
	if (itr != mResources.end())
	{
		mResources.erase(itr);

		const auto itrString = mResourceStrings.find(id);
		assert(itrString != mResourceStrings.end());
		mResourceStrings.erase(itrString);
	}
}

const std::string& ResourceManager::GetIdentifier(ResourceID id) const
{
	return mResourceStrings.at(id);
}

void ResourceManager::ReleaseAll()
{
	mResources.clear();
	mResourceStrings.clear();
}

U32 ResourceManager::Count() const
{
	return static_cast<U32>(mResources.size());
}

} // namespace en
