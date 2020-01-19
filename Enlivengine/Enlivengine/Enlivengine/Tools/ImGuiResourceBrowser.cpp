#include <Enlivengine/Tools/ImGuiResourceBrowser.hpp>

#ifdef ENLIVE_ENABLE_IMGUI

#include <imgui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <filesystem>

#include <Enlivengine/Application/PathManager.hpp>
#include <Enlivengine/Graphics/LinearColor.hpp>
#include <Enlivengine/System/ParserXml.hpp>

#include <Enlivengine/Application/AudioSystem.hpp>
#include <Enlivengine/Graphics/SFMLResources.hpp>

namespace en
{

ImGuiResourceBrowser::ImGuiResourceBrowser()
	: ImGuiTool()
{
	ImGuiFileDialog::Instance()->SetFilterColor(".ttf", ResourceInfo::ResourceInfoTypeToColor(ResourceInfo::Type::Font).withAlpha(0.7f).toImGuiColor());

	ImGuiFileDialog::Instance()->SetFilterColor(".tmx", LinearColor::Lime.withAlpha(0.7f).toImGuiColor());

	ImGuiFileDialog::Instance()->SetFilterColor(".ogg", ResourceInfo::ResourceInfoTypeToColor(ResourceInfo::Type::Music).withAlpha(0.7f).toImGuiColor());

	ImGuiFileDialog::Instance()->SetFilterColor(".wav", ResourceInfo::ResourceInfoTypeToColor(ResourceInfo::Type::Sound).withAlpha(0.7f).toImGuiColor());

	ImGuiFileDialog::Instance()->SetFilterColor(".png", ResourceInfo::ResourceInfoTypeToColor(ResourceInfo::Type::Texture).withAlpha(0.7f).toImGuiColor());
	ImGuiFileDialog::Instance()->SetFilterColor(".jpg", ResourceInfo::ResourceInfoTypeToColor(ResourceInfo::Type::Texture).withAlpha(0.7f).toImGuiColor());

	ImGuiFileDialog::Instance()->SetFilterColor(".cpp", LinearColor::Yellow.withAlpha(0.7f).toImGuiColor());
	ImGuiFileDialog::Instance()->SetFilterColor(".h", LinearColor::Yellow.withAlpha(0.7f).toImGuiColor());
	ImGuiFileDialog::Instance()->SetFilterColor(".hpp", LinearColor::Yellow.withAlpha(0.7f).toImGuiColor());
	ImGuiFileDialog::Instance()->SetFilterColor(".inl", LinearColor::Yellow.withAlpha(0.7f).toImGuiColor());

	ImGuiFileDialog::Instance()->SetFilterColor(".xml", LinearColor::LightBlue.withAlpha(0.7f).toImGuiColor());
	ImGuiFileDialog::Instance()->SetFilterColor(".txt", LinearColor::Magenta.withAlpha(0.7f).toImGuiColor());
	ImGuiFileDialog::Instance()->SetFilterColor(".md", LinearColor::Mint.withAlpha(0.7f).toImGuiColor());
}

ImGuiToolTab ImGuiResourceBrowser::GetTab() const
{
	return ImGuiToolTab::Main;
}

const char* ImGuiResourceBrowser::GetName() const
{
	return ICON_FA_COMPASS " ResourceBrowser";
}

void ImGuiResourceBrowser::Display()
{
	static const std::string key = "ResourceBrowserFileDialogKey";
	static const char* dialogTitle = "ResourceBrowser: Choose File";

	static std::filesystem::path assetsPath = PathManager::GetInstance().GetAssetsPath();

	ImGui::Text("Add New Resource");
	ImGui::Indent(); 
	ImGui::InputText("Identitifer", mIdentifierBuffer, kBufferSize);
	if (ImGui::Button("..."))
	{
		if (!assetsPath.is_absolute())
		{
			assetsPath = std::filesystem::absolute(assetsPath);
		}
		ImGuiFileDialog::Instance()->OpenDialog(key, dialogTitle, nullptr, assetsPath.string());
	}
	ImGui::SameLine();
	ImGui::InputText("Filename", mFilenameBuffer, kBufferSize);
	if (ImGuiFileDialog::Instance()->FileDialog(key))
	{
		if (ImGuiFileDialog::Instance()->IsOk)
		{
			std::filesystem::path filepath = ImGuiFileDialog::Instance()->GetFilepathName();
			std::string relativeResult = filepath.lexically_relative(assetsPath).string();
			std::replace(relativeResult.begin(), relativeResult.end(), '\\', '/');

#ifdef ENLIVE_COMPILER_MSVC
			strcpy_s(mFilenameBuffer, relativeResult.c_str());
#else
			strcpy(mFilenameBuffer, relativeResult..c_str());
#endif // ENLIVE_COMPILER_MSVC
		}
		ImGuiFileDialog::Instance()->CloseDialog(key);
	}
	if (strlen(mIdentifierBuffer) > 0 && strlen(mFilenameBuffer) > 0)
	{
		if (ImGui::Button("Add"))
		{
			mResourceInfos.push_back(ResourceInfo());
			ResourceInfo& resourceInfo = mResourceInfos.back();
			resourceInfo.identifier = mIdentifierBuffer;
			resourceInfo.filename = mFilenameBuffer;
			resourceInfo.type = ResourceInfo::DetectTypeFromFilename(resourceInfo.filename);

			resourceInfo.Load();

			std::sort(mResourceInfos.begin(), mResourceInfos.end(), [](const ResourceInfo& a, const ResourceInfo& b) { return a.type < b.type; });

			AskForResize();

			SaveResourceInfosToFile(PathManager::GetInstance().GetAssetsPath() + "resources.xml");

#ifdef ENLIVE_COMPILER_MSVC
			strcpy_s(mIdentifierBuffer, "");
			strcpy_s(mFilenameBuffer, "");
#else
			strcpy(mIdentifierBuffer, "");
			strcpy(mFilenameBuffer, "");
#endif // ENLIVE_COMPILER_MSVC
		}
	}
	else
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		ImGui::Button("Add");
		ImGui::PopStyleVar();
	}
	ImGui::Unindent();

	U32 size = static_cast<U32>(mResourceInfos.size());
	if (size > 0)
	{
		ImGui::Separator();

		ImGui::BeginChild("", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		ResourceInfo::Type previousType = mResourceInfos[0].type;
		for (U32 i = 0; i < size; ++i)
		{
			const ResourceInfo& resourceInfo = mResourceInfos[i];
			if (resourceInfo.type > previousType)
			{
				ImGui::Separator();
				previousType = resourceInfo.type;
			}

			ImGui::PushID(i);

			if (resourceInfo.type == ResourceInfo::Type::Unknown)
			{
				ImGui::Text(ICON_FA_EXCLAMATION_TRIANGLE);
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(4, 0));
				ImGui::SameLine();
			}

			ImVec4 color = ResourceInfo::ResourceInfoTypeToColor(resourceInfo.type).toImGuiColor();

			ImGui::TextColored(color, resourceInfo.identifier.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("ID: %d", resourceInfo.resourceID);
			}
			ImGui::SameLine();
			ImGui::Text(" : %s", resourceInfo.filename.c_str());
			ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				if (resourceInfo.resourceID != InvalidResourceID)
				{
					if (resourceInfo.type == ResourceInfo::Type::Sound)
					{
						AudioSystem::GetInstance().ReleaseSound(resourceInfo.resourceID);
					}
					else
					{
						ResourceManager::GetInstance().Release(resourceInfo.resourceID);
					}
				}

				mResourceInfos.erase(mResourceInfos.begin() + i);
				size--;
				i--;
			}

			ImGui::PopID();
		}
		ImGui::EndChild();
	}
}

bool ImGuiResourceBrowser::LoadResourceInfosFromFile(const std::string& filename)
{
	ParserXml xml;
	if (!xml.loadFromFile(filename))
	{
		LogError(en::LogChannel::Application, 9, "Can't open resources file at %s", filename.c_str());
		return false;
	}

	if (xml.readNode("Resources"))
	{
		U32 resourceCount = 0;
		xml.getAttribute("resourceCount", resourceCount);
		U32 typeCount = 0;
		xml.getAttribute("typeCount", typeCount);
		const bool needToRecomputeTypesFromFilename = (typeCount == static_cast<U32>(ResourceInfo::Type::Count));
		if (resourceCount > 0)
		{
			mResourceInfos.reserve(resourceCount);
			if (xml.readNode("Resource"))
			{
				do
				{
					mResourceInfos.push_back(ResourceInfo());
					ResourceInfo& resourceInfo = mResourceInfos.back();
					xml.getAttribute("identifier", resourceInfo.identifier);
					xml.getAttribute("filename", resourceInfo.filename);
					I32 typeInt;
					xml.getAttribute("type", typeInt);
					if (!needToRecomputeTypesFromFilename)
					{
						resourceInfo.type = static_cast<ResourceInfo::Type>(typeInt);
					}
					else
					{
						resourceInfo.type = ResourceInfo::DetectTypeFromFilename(resourceInfo.filename);
					}
					resourceInfo.Load();
				} while (xml.nextSibling("Resource"));
				xml.closeNode();
			}
		}
		xml.closeNode();
	}

	std::sort(mResourceInfos.begin(), mResourceInfos.end(), [](const ResourceInfo& a, const ResourceInfo& b) { return a.type < b.type; });

	AskForResize();

	return true;
}

bool ImGuiResourceBrowser::SaveResourceInfosToFile(const std::string& filename)
{
	ParserXml xml;
	xml.newFile();

	if (!xml.createChild("Resources"))
	{
		return false;
	}
	xml.setAttribute("resourceCount", static_cast<U32>(mResourceInfos.size()));
	xml.setAttribute("typeCount", static_cast<U32>(ResourceInfo::Type::Count));

	for (const ResourceInfo& resourceInfo : mResourceInfos)
	{
		if (!xml.createChild("Resource"))
		{
			continue;
		}

		xml.setAttribute("identifier", resourceInfo.identifier);
		xml.setAttribute("filename", resourceInfo.filename);
		xml.setAttribute("type", static_cast<I32>(resourceInfo.type));

		xml.closeNode();
	}

	if (!xml.saveToFile(filename))
	{
		return false;
	}

	return true;
}

bool ImGuiResourceBrowser::ResourceInfo::Load()
{
	ResourceManager& resourceManager = ResourceManager::GetInstance();
	std::string resourceFilename = en::PathManager::GetInstance().GetAssetsPath() + filename;
	switch (type)
	{
	case ResourceInfo::Type::Unknown:
	{
		resourceID = InvalidResourceID;
		LogError(en::LogChannel::Global, 4, "Unknown resource type for resource : %s, %s", identifier.c_str(), resourceFilename.c_str());
		return false;
	}
	case ResourceInfo::Type::Font:
	{
		FontPtr fontPtr = resourceManager.Create<Font>(identifier.c_str(), FontLoader::FromFile(resourceFilename));
		if (!fontPtr.IsValid())
		{
			resourceID = InvalidResourceID;
			LogWarning(en::LogChannel::Global, 2, "Can't load resource : %s, %s", identifier.c_str(), resourceFilename.c_str());
			return false;
		}
		else
		{
			resourceID = fontPtr.GetID();
		}
		break;
	}
	case ResourceInfo::Type::Music:
	{
		MusicID musicID = AudioSystem::GetInstance().PrepareMusic(identifier.c_str(), resourceFilename);
		if (musicID == InvalidMusicID)
		{
			resourceID = InvalidResourceID;
			LogWarning(en::LogChannel::Global, 2, "Can't load resource : %s, %s", identifier.c_str(), resourceFilename.c_str());
			return false;
		}
		else
		{
			resourceID = musicID;
		}
		break;
	}
	case ResourceInfo::Type::Sound:
	{
		SoundID soundID = AudioSystem::GetInstance().PrepareSound(identifier.c_str(), resourceFilename);
		if (soundID == InvalidSoundID)
		{
			resourceID = InvalidResourceID;
			LogWarning(en::LogChannel::Global, 2, "Can't load resource : %s, %s", identifier.c_str(), resourceFilename.c_str());
			return false;
		}
		else
		{
			resourceID = soundID;
		}
		break;
	}
	case ResourceInfo::Type::Texture:
	{
		TexturePtr texturePtr = resourceManager.Create<Texture>(identifier.c_str(), TextureLoader::FromFile(resourceFilename));
		if (!texturePtr.IsValid())
		{
			resourceID = InvalidResourceID;
			LogWarning(en::LogChannel::Global, 2, "Can't load resource : %s, %s", identifier.c_str(), resourceFilename.c_str());
			return false;
		}
		else
		{
			resourceID = texturePtr.GetID();
		}
		break;
	}
	default: assert(false); return false;
	}
	return true;
}

ImGuiResourceBrowser::ResourceInfo::Type ImGuiResourceBrowser::ResourceInfo::DetectTypeFromFilename(const std::string& filename)
{
	std::string ext = std::filesystem::path(filename).extension().string();
	if (ext == ".ttf")
	{
		return Type::Font;
	}
	if (ext == ".ogg")
	{
		return Type::Music;
	}
	if (ext == ".wav")
	{
		return Type::Sound;
	}
	if (ext == ".png" || ext == ".jpg")
	{
		return Type::Texture;
	}
	return Type::Unknown;
}

const char* ImGuiResourceBrowser::ResourceInfo::ResourceInfoTypeToString(Type type)
{
	switch (type)
	{
	case ResourceInfo::Type::Unknown: return "Unknown"; break;
	case ResourceInfo::Type::Font: return "Font"; break;
	case ResourceInfo::Type::Music: return "Music"; break;
	case ResourceInfo::Type::Sound: return "Sound"; break;
	case ResourceInfo::Type::Texture: return "Texture"; break;
	default: assert(false); break;
	}
	return "";
}

const LinearColor& ImGuiResourceBrowser::ResourceInfo::ResourceInfoTypeToColor(Type type)
{
	static const LinearColor& unknownColor = LinearColor::Red;
	static LinearColor resourceInfoTypeColors[static_cast<U32>(ResourceInfo::Type::Count)] = 
	{
		LinearColor::Orange, // Font
		LinearColor::BabyPink, // Music
		LinearColor::HotPink, // Sound
		LinearColor::Cyan, // Texture
	};
	if (type == ResourceInfo::Type::Unknown)
	{
		return unknownColor;
	}
	else
	{
		return resourceInfoTypeColors[static_cast<U32>(type)];
	}
}

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
