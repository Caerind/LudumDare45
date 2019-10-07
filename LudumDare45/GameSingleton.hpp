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

		static void setCursor(bool enable);

		static void clear();

		static void playSound(en::ResourceId r);

		static en::Application* application;

		static bool mFirstIntroDone;
		static bool mFirstThrowNothingDone;

		static bool soundEnabled;

		static en::Tileset mTileset;
		static GameMap mMap;
		static en::View mView;

		static entt::registry world;
		static entt::entity playerEntity;
		static entt::entity nothingEntity;

		static en::ResourceId mFont;

		static en::ResourceId mNothingTexture;
		static en::ResourceId mEverythingTexture;
		static en::ResourceId mAITexture;

		static en::ResourceId mChopSound;
		static en::ResourceId mHitSound;
		static en::ResourceId mKnockoutSound;
		static en::ResourceId mSelectSound;
		static en::ResourceId mThrowSound;
		static en::ResourceId mDestrPropsSound;
		static en::ResourceId mNothingWallSound;
		static en::ResourceId mNothingAISound;
		static en::ResourceId mPieceSound;
		static en::ResourceId mPieceGetSound;

		static std::vector<en::Animation> mAnimations;

		static en::F32 hitLife;
		static en::F32 aiNothingLife;
		static en::F32 nothingMurLife;
		static en::F32 nothingPropsLife;
		static en::F32 nothingAILife;

#ifdef ENLIVE_ENABLE_IMGUI
		static entt::entity currentEntity;
		static ImGuiEntityEditor<entt::registry> worldEditor;

		static bool showWindow;
		static void renderMetricsWindow();
#endif // ENLIVE_ENABLE_IMGUI
};
