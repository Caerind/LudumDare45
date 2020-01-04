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

		static void playSound(en::ResourceID r);

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

		static en::FontPtr mFont;

		static en::TexturePtr mNothingTexture;
		static en::TexturePtr mEverythingTexture;
		static en::TexturePtr mAITexture;

		static en::SoundId mChopSound;
		static en::SoundId mHitSound;
		static en::SoundId mKnockoutSound;
		static en::SoundId mSelectSound;
		static en::SoundId mThrowSound;
		static en::SoundId mDestrPropsSound;
		static en::SoundId mNothingWallSound;
		static en::SoundId mNothingAISound;
		static en::SoundId mPieceSound;
		static en::SoundId mPieceGetSound;

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
