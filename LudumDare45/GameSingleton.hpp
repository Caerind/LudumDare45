#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Config.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/ResourceManager.hpp>
#include <Enlivengine/Graphics/SFMLResources.hpp>
#include <Enlivengine/Graphics/Tileset.hpp>
#include <Enlivengine/Graphics/Animation.hpp>
#include <Enlivengine/Application/AudioSystem.hpp>
#include <entt/entt.hpp>

#include "GameConfig.hpp"
#include "GameMap.hpp"

#include <vector>

#include "imgui_entt_entity_editor.hpp"

class GameSingleton
{
	public:
		static void loadResourcesMain(en::Application& application);
		static void loadResourcesGame();
		static void loadAnimations();

		static void clear();

		static void playSound(en::ResourceId r, const en::Vector2f& position);
		static void playClick();

		static en::Application* application;

		static en::Tileset mTileset;
		static GameMap mMap;
		static en::View mView;

		static entt::registry world;
		static entt::entity playerEntity;
		static entt::entity nothingEntity;

		static en::ResourceId mFont;

		static en::ResourceId mNothingTexture;
		static en::ResourceId mEverythingTexture;

		static en::ResourceId mClickSound;

		static std::vector<en::Animation> mAnimations;

		static en::U32 mMoney;

#ifdef ENLIVE_ENABLE_IMGUI
		static entt::entity currentEntity;
		static ImGuiEntityEditor<entt::registry> worldEditor;

		static bool showWindow;
		static void renderMetricsWindow();
#endif // ENLIVE_ENABLE_IMGUI
};
