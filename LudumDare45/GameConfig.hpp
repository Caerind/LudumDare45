#pragma once

#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/System/Assert.hpp>
#include <Enlivengine/System/ParserIni.hpp>
#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Config.hpp>

#include <string>

class GameConfig
{
	public:
		static std::string configFilename;

		static std::string getS(const std::string& index);
		static en::I32 getI(const std::string& index);
		static en::F32 getF(const std::string& index);
		static en::U32 getU(const std::string& index);

		static void setS(const std::string& index, const std::string& value);
		static void setI(const std::string& index, en::I32 value);
		static void setF(const std::string& index, en::F32 value);
		static void setU(const std::string& index, en::U32 value);

		static bool loadFromFile(const std::string& filename);
		static bool saveToFile(const std::string& filename);
		static void loadFromInternal();

		static en::ParserIni mIni;

#ifdef ENLIVE_ENABLE_IMGUI
		static bool showWindow;
		static void renderConfigWindow();
#endif // ENLIVE_ENABLE_IMGUI
};