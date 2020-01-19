#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>

#ifdef ENLIVE_ENABLE_IMGUI

#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Application/ResourceManager.hpp>

namespace en
{

class ImGuiResourceBrowser : public ImGuiTool
{
	ENLIVE_SINGLETON(ImGuiResourceBrowser);

public:
	virtual ImGuiToolTab GetTab() const;
	virtual const char* GetName() const;

	virtual void Display();

	bool LoadResourceInfosFromFile(const std::string& filename);
	bool SaveResourceInfosToFile(const std::string& filename);

private:
	static constexpr U32 kBufferSize{ 256 };
	char mIdentifierBuffer[kBufferSize];
	char mFilenameBuffer[kBufferSize];

	struct ResourceInfo
	{
		enum class Type
		{
			Unknown = -1,
			Font,
			Music,
			Sound,
			Texture,
			Count
		};

		std::string identifier;
		std::string filename;
		Type type;
		ResourceID resourceID;

		bool Load();

		static Type DetectTypeFromFilename(const std::string& filename);
		static const char* ResourceInfoTypeToString(Type type);
		static const LinearColor& ResourceInfoTypeToColor(Type type);
	};
	std::vector<ResourceInfo> mResourceInfos;
};

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
