#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Enlivengine/System/PrimitiveTypes.hpp>

namespace en
{

using ResourceID = U32;
constexpr ResourceID InvalidResourceID = U32_Max;

namespace priv
{

ResourceID StringToResourceID(const char* str);

class BaseResource
{
public:
	BaseResource();

	bool IsLoaded() const;
	bool IsManaged() const;
	ResourceID GetID() const;

public:
	friend class ResourceManager;
	ResourceID mID;

public:
	bool mLoaded;
};

} // namespace priv

class ResourceManager;

template <typename T>
class ResourcePtr
{
public:
	ResourcePtr(ResourceID id = InvalidResourceID, ResourceManager* mgr = nullptr);

	ResourceID GetID() const;
	bool IsValid() const;
	operator bool() const;
	void Release();

	T* GetPtr();
	const T* GetPtr() const;
	T& Get();
	const T& Get() const;

	void ReleaseFromManager();

private:
	ResourceID mID;
	ResourceManager* mManager;
};

template <typename T>
class Resource : public priv::BaseResource
{
public:
	using Ptr = ResourcePtr<T>;

public:
	Resource();
};

template <typename T>
class ResourceLoader
{
public:
	using LoaderFunc = std::function<bool(T&)>;

public:
	ResourceLoader(LoaderFunc loader);

	bool Load(T& resource) const;

private:
	LoaderFunc mFunc;
};

enum class ResourceKnownStrategy
{
	Reuse = 0,
	Reload,
	Null
};

class ResourceHolder;

class ResourceManager
{
public:
	ResourceManager();

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	template <typename T> ResourcePtr<T> Create(const char* str, const ResourceLoader<T>& loader, ResourceKnownStrategy knownStrategy = ResourceKnownStrategy::Reuse);
	template <typename T> ResourcePtr<T> Get(const char* str);
	template <typename T> ResourcePtr<T> Get(ResourceID id);

	bool Has(const char* str) const;
	bool Has(ResourceID id) const;

	void Release(const char* str);
	void Release(ResourceID id);

	void ReleaseAll();

	U32 Count() const;

private:
	template <typename T> friend class ResourcePtr;
	template <typename T> T* GetRawPtr(ResourceID id);
	template <typename T> const T* GetRawPtr(ResourceID id) const;

private:
	std::unordered_map<ResourceID, std::unique_ptr<priv::BaseResource>> mResources;
};

} // namespace en

#include "ResourceManager.inl"
