#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Config.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/ResourceManager.hpp>
#include <Enlivengine/Graphics/SFMLResources.hpp>
#include <Enlivengine/Application/AudioSystem.hpp>
#include <entt/entt.hpp>

#include "GameConfig.hpp"

#include "imgui_entt_entity_editor.hpp"

class GameSingleton
{
	public:
		static void loadResourcesMain(en::Application& application);
		static void loadResourcesGame();

		static void clear();

		static void playSound(en::ResourceId r, const en::Vector2f& position);
		static void playClick();

		static en::Application* application;

		static entt::registry world;

		static en::ResourceId mFont;

		static en::ResourceId mIndicatorTexture;
		static en::ResourceId mTilesetTexture;

		static en::ResourceId mAsteroidSound;
		static en::ResourceId mExplosionSound;
		static std::shared_ptr<sf::Sound> mEngineSoundManager;

		static en::U32 mMoney;
		static en::U32 mResource1;
		static en::U32 mResource2;
		static en::U32 mResource3;

#ifdef ENLIVE_ENABLE_IMGUI
		static entt::entity currentEntity;
		static ImGuiEntityEditor<entt::registry> worldEditor;

		static bool showWindow;
		static void renderMetricsWindow();
#endif // ENLIVE_ENABLE_IMGUI
};
