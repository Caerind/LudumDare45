#include <Enlivengine/Application/ResourceManager.hpp>

namespace en
{

namespace priv
{

ResourceID StringToResourceID(const char* str)
{
	return Hash::CRC32(str);
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

bool ResourceManager::Has(const char* str) const
{
	return Has(priv::StringToResourceID(str));
}

bool ResourceManager::Has(ResourceID id) const
{
	return mResources.find(id) != mResources.end();
}

void ResourceManager::Release(const char* str)
{
	Release(priv::StringToResourceID(str));
}

void ResourceManager::Release(ResourceID id)
{
	const auto itr = mResources.find(id);
	if (itr != mResources.end())
	{
		mResources.erase(itr);
	}
}

void ResourceManager::ReleaseAll()
{
	mResources.clear();
}

U32 ResourceManager::Count() const
{
	return static_cast<U32>(mResources.size());
}

} // namespace en
