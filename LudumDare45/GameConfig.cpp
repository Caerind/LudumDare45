#include "GameConfig.hpp"

#ifdef ENLIVE_ENABLE_IMGUI
#include <imgui/imgui.h>
#endif

std::string GameConfig::configFilename = "Assets/Data/config.ini";
en::ParserIni GameConfig::mIni;

#ifdef ENLIVE_ENABLE_IMGUI
bool GameConfig::showWindow = false;
#endif

std::string GameConfig::getS(const std::string & index)
{
	return mIni.get(index);
}

en::I32 GameConfig::getI(const std::string & index)
{
	return std::stoi(getS(index));
}

en::F32 GameConfig::getF(const std::string & index)
{
	return std::stof(getS(index));
}

en::U32 GameConfig::getU(const std::string & index)
{
	return std::stoul(getS(index));
}

void GameConfig::setS(const std::string & index, const std::string & value)
{
	mIni.set(value, index);
}

void GameConfig::setI(const std::string & index, en::I32 value)
{
	setS(index, std::to_string(value));
}

void GameConfig::setF(const std::string & index, en::F32 value)
{
	setS(index, std::to_string(value));
}

void GameConfig::setU(const std::string & index, en::U32 value)
{
	setS(index, std::to_string(value));
}

bool GameConfig::loadFromFile(const std::string & filename)
{
	return mIni.loadFromFile(filename);
}

bool GameConfig::saveToFile(const std::string & filename)
{
	return mIni.saveToFile(filename);
}

void GameConfig::loadFromInternal()
{
	// TODO : GameConfig::loadFromInternal()
}

#ifdef ENLIVE_ENABLE_IMGUI
void GameConfig::renderConfigWindow()
{
	if (showWindow)
	{
		ImGui::Begin("GameConfig");

		ImGui::BeginChild("Settings");

		if (ImGui::Button("AABB"))
		{
			/*
			if (GameConfig::getU("aabb") == 1)
			{
				GameConfig::setU("aabb", 0);
			}
			else
			{
				GameConfig::setU("aabb", 1);
			}
			*/
		}

		ImGui::Separator();

		en::F32 var = 0.0f;

		#define CONF_SETTING(str, min, max) \
		var = GameConfig::getF(str); \
		ImGui::SliderFloat(str, &var, min, max); \
		GameConfig::setF(str, var);

		ImGui::SliderFloat("A", &var, 500.f, 5000.f);
		ImGui::SliderFloat("A", &var, 500.f, 5000.f);
		//CONF_SETTING("event1-var", 500, 5000);

		ImGui::Separator();

		ImGui::SliderFloat("B", &var, 0.5f, 4.0f);
		ImGui::SliderFloat("B", &var, 0.5f, 4.0f);
		//CONF_SETTING("player-regen", 0.5f, 4.0f);

		ImGui::Separator();

		ImGui::SliderFloat("C", &var, 50.f, 1000.f);
		ImGui::SliderFloat("C", &var, 50.f, 1000.f);
		//CONF_SETTING("enemy1-life", 50, 1000);

		ImGui::Separator();

		ImGui::SliderFloat("D", &var, 0.5f, 4.0f);
		ImGui::SliderFloat("D", &var, 0.5f, 4.0f);
		//CONF_SETTING("planetgen-len-a", 500, 5000);

		#undef CONF_SETTING

		ImGui::EndChild();

		ImGui::End();
	}
}
#endif // ENLIVE_ENABLE_IMGUI