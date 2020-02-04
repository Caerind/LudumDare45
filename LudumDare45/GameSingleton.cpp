#include "GameSingleton.hpp"

#ifdef ENLIVE_ENABLE_IMGUI
#include <imgui/imgui.h>
#endif

#ifdef ENLIVE_DEBUG
#include <Enlivengine/Graphics/DebugDraw.hpp>
#endif

#include <Enlivengine/Graphics/SFMLResources.hpp>

en::Application* GameSingleton::application;

bool GameSingleton::mFirstIntroDone = false;
bool GameSingleton::mFirstThrowNothingDone = false;

GameMap GameSingleton::mMap;
en::View GameSingleton::mView;

entt::registry GameSingleton::world;
entt::entity GameSingleton::playerEntity;
entt::entity GameSingleton::nothingEntity;

en::FontPtr GameSingleton::mFont;

en::TexturePtr GameSingleton::mNothingTexture;
en::TexturePtr GameSingleton::mEverythingTexture;
en::TexturePtr GameSingleton::mAITexture;

en::SoundID GameSingleton::mChopSound;
en::SoundID GameSingleton::mHitSound;
en::SoundID GameSingleton::mKnockoutSound;
en::SoundID GameSingleton::mSelectSound;
en::SoundID GameSingleton::mThrowSound;
en::SoundID GameSingleton::mDestrPropsSound;
en::SoundID GameSingleton::mNothingWallSound;
en::SoundID GameSingleton::mNothingAISound;
en::SoundID GameSingleton::mPieceSound;
en::SoundID GameSingleton::mPieceGetSound;

std::vector<en::Animation> GameSingleton::mAnimations;

en::F32 GameSingleton::hitLife = 0.03f;
en::F32 GameSingleton::aiNothingLife = 1.0f;
en::F32 GameSingleton::nothingMurLife = 0.15f;
en::F32 GameSingleton::nothingPropsLife = 0.10f;
en::F32 GameSingleton::nothingAILife = 0.05f;

#ifdef ENLIVE_ENABLE_IMGUI
entt::entity GameSingleton::currentEntity = entt::null;
ImGuiEntityEditor<entt::registry> GameSingleton::worldEditor;
bool GameSingleton::showWindow = false;
#endif

void GameSingleton::loadResourcesMain(en::Application& app)
{
	GameSingleton::application = &app;
	mFont = en::ResourceManager::GetInstance().Get<en::Font>("MainFont");
}

void GameSingleton::loadResourcesGame()
{
	// Textures
	const std::string& texturesPath = en::PathManager::GetInstance().GetTexturesPath();
	mNothingTexture = en::ResourceManager::GetInstance().Get<en::Texture>("nothingtexture");
	mEverythingTexture = en::ResourceManager::GetInstance().Get<en::Texture>("everythingtexture");
	mAITexture = en::ResourceManager::GetInstance().Get<en::Texture>("aitexture");

	// Sound
	const std::string& soundsPath = en::PathManager::GetInstance().GetSoundsPath();
	mChopSound = en::AudioSystem::GetInstance().PrepareSound("chop", soundsPath + "Chop.wav");
	mHitSound = en::AudioSystem::GetInstance().PrepareSound("hit", soundsPath + "Hit.wav");
	mKnockoutSound = en::AudioSystem::GetInstance().PrepareSound("knockout", soundsPath + "Knockout.wav");
	mSelectSound = en::AudioSystem::GetInstance().PrepareSound("select", soundsPath + "Select.wav");
	mThrowSound = en::AudioSystem::GetInstance().PrepareSound("throw", soundsPath + "Throw.wav");
	mDestrPropsSound = en::AudioSystem::GetInstance().PrepareSound("props", soundsPath + "Props.wav");
	mNothingWallSound = en::AudioSystem::GetInstance().PrepareSound("nwall", soundsPath + "Nwall.wav");
	mNothingAISound = en::AudioSystem::GetInstance().PrepareSound("nai", soundsPath + "Nia.wav");
	mPieceSound = en::AudioSystem::GetInstance().PrepareSound("piece", soundsPath + "Piece.wav");
	mPieceGetSound = en::AudioSystem::GetInstance().PrepareSound("pieceGet", soundsPath + "PieceGet.wav");

	// Idle
	en::Animation animIdleBG;
	animIdleBG.addFrame(en::Animation::Frame(0, en::Recti(0, 0, 16, 16), en::seconds(0.3f)));
	animIdleBG.addFrame(en::Animation::Frame(0, en::Recti(16, 0, 16, 16), en::seconds(0.3f)));
	animIdleBG.addFrame(en::Animation::Frame(0, en::Recti(32, 0, 16, 16), en::seconds(0.3f)));
	mAnimations.push_back(animIdleBG);
	en::Animation animIdleBD;
	animIdleBD.addFrame(en::Animation::Frame(0, en::Recti(0, 16, 16, 16), en::seconds(0.3f)));
	animIdleBD.addFrame(en::Animation::Frame(0, en::Recti(16, 16, 16, 16), en::seconds(0.3f)));
	animIdleBD.addFrame(en::Animation::Frame(0, en::Recti(32, 16, 16, 16), en::seconds(0.3f)));
	mAnimations.push_back(animIdleBD);
	en::Animation animIdleHG;
	animIdleHG.addFrame(en::Animation::Frame(0, en::Recti(0, 32, 16, 16), en::seconds(0.3f)));
	animIdleHG.addFrame(en::Animation::Frame(0, en::Recti(16, 32, 16, 16), en::seconds(0.3f)));
	animIdleHG.addFrame(en::Animation::Frame(0, en::Recti(32, 32, 16, 16), en::seconds(0.3f)));
	mAnimations.push_back(animIdleHG);
	en::Animation animIdleHD;
	animIdleHD.addFrame(en::Animation::Frame(0, en::Recti(0, 48, 16, 16), en::seconds(0.3f)));
	animIdleHD.addFrame(en::Animation::Frame(0, en::Recti(16, 48, 16, 16), en::seconds(0.3f)));
	animIdleHD.addFrame(en::Animation::Frame(0, en::Recti(32, 48, 16, 16), en::seconds(0.3f)));
	mAnimations.push_back(animIdleHD);

	// Hitted
	en::Animation animHittedBG;
	animHittedBG.addFrame(en::Animation::Frame(0, en::Recti(0, 64, 16, 16), en::seconds(0.03f)));
	animHittedBG.addFrame(en::Animation::Frame(0, en::Recti(16, 64, 16, 16), en::seconds(0.03f)));
	animHittedBG.addFrame(en::Animation::Frame(0, en::Recti(32, 64, 16, 16), en::seconds(0.03f)));
	animHittedBG.addFrame(en::Animation::Frame(0, en::Recti(48, 64, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHittedBG);
	en::Animation animHittedBD;
	animHittedBD.addFrame(en::Animation::Frame(0, en::Recti(0, 80, 16, 16), en::seconds(0.03f)));
	animHittedBD.addFrame(en::Animation::Frame(0, en::Recti(16, 80, 16, 16), en::seconds(0.03f)));
	animHittedBD.addFrame(en::Animation::Frame(0, en::Recti(32, 80, 16, 16), en::seconds(0.03f)));
	animHittedBD.addFrame(en::Animation::Frame(0, en::Recti(48, 80, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHittedBD);
	en::Animation animHittedHG;
	animHittedHG.addFrame(en::Animation::Frame(0, en::Recti(0, 96, 16, 16), en::seconds(0.03f)));
	animHittedHG.addFrame(en::Animation::Frame(0, en::Recti(16, 96, 16, 16), en::seconds(0.03f)));
	animHittedHG.addFrame(en::Animation::Frame(0, en::Recti(32, 96, 16, 16), en::seconds(0.03f)));
	animHittedHG.addFrame(en::Animation::Frame(0, en::Recti(48, 96, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHittedHG);
	en::Animation animHittedHD;
	animHittedHD.addFrame(en::Animation::Frame(0, en::Recti(0, 112, 16, 16), en::seconds(0.03f)));
	animHittedHD.addFrame(en::Animation::Frame(0, en::Recti(16, 112, 16, 16), en::seconds(0.03f)));
	animHittedHD.addFrame(en::Animation::Frame(0, en::Recti(32, 112, 16, 16), en::seconds(0.03f)));
	animHittedHD.addFrame(en::Animation::Frame(0, en::Recti(48, 112, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHittedHD);

	// Walking
	en::Animation animWalkingBG;
	animWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(0, 128, 16, 16), en::seconds(0.1f)));
	animWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(16, 128, 16, 16), en::seconds(0.1f)));
	animWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(32, 128, 16, 16), en::seconds(0.1f)));
	animWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(48, 128, 16, 16), en::seconds(0.1f)));
	animWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(64, 128, 16, 16), en::seconds(0.1f)));
	animWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(80, 128, 16, 16), en::seconds(0.1f)));
	mAnimations.push_back(animWalkingBG);
	en::Animation animWalkingBD;
	animWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(0, 144, 16, 16), en::seconds(0.1f)));
	animWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(16, 144, 16, 16), en::seconds(0.1f)));
	animWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(32, 144, 16, 16), en::seconds(0.1f)));
	animWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(48, 144, 16, 16), en::seconds(0.1f)));
	animWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(64, 144, 16, 16), en::seconds(0.1f)));
	animWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(80, 144, 16, 16), en::seconds(0.1f)));
	mAnimations.push_back(animWalkingBD);
	en::Animation animWalkingHG;
	animWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(0, 160, 16, 16), en::seconds(0.1f)));
	animWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(16, 160, 16, 16), en::seconds(0.1f)));
	animWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(32, 160, 16, 16), en::seconds(0.1f)));
	animWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(48, 160, 16, 16), en::seconds(0.1f)));
	animWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(64, 160, 16, 16), en::seconds(0.1f)));
	animWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(80, 160, 16, 16), en::seconds(0.1f)));
	mAnimations.push_back(animWalkingHG);
	en::Animation animWalkingHD;
	animWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(0, 176, 16, 16), en::seconds(0.1f)));
	animWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(16, 176, 16, 16), en::seconds(0.1f)));
	animWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(32, 176, 16, 16), en::seconds(0.1f)));
	animWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(48, 176, 16, 16), en::seconds(0.1f)));
	animWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(64, 176, 16, 16), en::seconds(0.1f)));
	animWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(80, 176, 16, 16), en::seconds(0.1f)));
	mAnimations.push_back(animWalkingHD);

	// HitWalking
	en::Animation animHitWalkingBG;
	animHitWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 192, 16, 16), en::seconds(0.03f)));
	animHitWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 192, 16, 16), en::seconds(0.03f)));
	animHitWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 192, 16, 16), en::seconds(0.03f)));
	animHitWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 192, 16, 16), en::seconds(0.03f)));
	animHitWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 192, 16, 16), en::seconds(0.03f)));
	animHitWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 192, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitWalkingBG);
	en::Animation animHitWalkingBD;
	animHitWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 208, 16, 16), en::seconds(0.03f)));
	animHitWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 208, 16, 16), en::seconds(0.03f)));
	animHitWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 208, 16, 16), en::seconds(0.03f)));
	animHitWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 208, 16, 16), en::seconds(0.03f)));
	animHitWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 208, 16, 16), en::seconds(0.03f)));
	animHitWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 208, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitWalkingBD);
	en::Animation animHitWalkingHG;
	animHitWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 224, 16, 16), en::seconds(0.03f)));
	animHitWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 224, 16, 16), en::seconds(0.03f)));
	animHitWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 224, 16, 16), en::seconds(0.03f)));
	animHitWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 224, 16, 16), en::seconds(0.03f)));
	animHitWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 224, 16, 16), en::seconds(0.03f)));
	animHitWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 224, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitWalkingHG);
	en::Animation animHitWalkingHD;
	animHitWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 240, 16, 16), en::seconds(0.03f)));
	animHitWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 240, 16, 16), en::seconds(0.03f)));
	animHitWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 240, 16, 16), en::seconds(0.03f)));
	animHitWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 240, 16, 16), en::seconds(0.03f)));
	animHitWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 240, 16, 16), en::seconds(0.03f)));
	animHitWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 240, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitWalkingHD);

	// HitIdle
	en::Animation animHitIdleBG;
	animHitIdleBG.addFrame(en::Animation::Frame(0, en::Recti(0, 192, 16, 16), en::seconds(0.03f)));
	animHitIdleBG.addFrame(en::Animation::Frame(0, en::Recti(16, 192, 16, 16), en::seconds(0.03f)));
	animHitIdleBG.addFrame(en::Animation::Frame(0, en::Recti(32, 192, 16, 16), en::seconds(0.03f)));
	animHitIdleBG.addFrame(en::Animation::Frame(0, en::Recti(48, 192, 16, 16), en::seconds(0.03f)));
	animHitIdleBG.addFrame(en::Animation::Frame(0, en::Recti(64, 192, 16, 16), en::seconds(0.03f)));
	animHitIdleBG.addFrame(en::Animation::Frame(0, en::Recti(80, 192, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitIdleBG);
	en::Animation animHitIdleBD;
	animHitIdleBD.addFrame(en::Animation::Frame(0, en::Recti(0, 208, 16, 16), en::seconds(0.03f)));
	animHitIdleBD.addFrame(en::Animation::Frame(0, en::Recti(16, 208, 16, 16), en::seconds(0.03f)));
	animHitIdleBD.addFrame(en::Animation::Frame(0, en::Recti(32, 208, 16, 16), en::seconds(0.03f)));
	animHitIdleBD.addFrame(en::Animation::Frame(0, en::Recti(48, 208, 16, 16), en::seconds(0.03f)));
	animHitIdleBD.addFrame(en::Animation::Frame(0, en::Recti(64, 208, 16, 16), en::seconds(0.03f)));
	animHitIdleBD.addFrame(en::Animation::Frame(0, en::Recti(80, 208, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitIdleBD);
	en::Animation animHitIdleHG;
	animHitIdleHG.addFrame(en::Animation::Frame(0, en::Recti(0, 224, 16, 16), en::seconds(0.03f)));
	animHitIdleHG.addFrame(en::Animation::Frame(0, en::Recti(16, 224, 16, 16), en::seconds(0.03f)));
	animHitIdleHG.addFrame(en::Animation::Frame(0, en::Recti(32, 224, 16, 16), en::seconds(0.03f)));
	animHitIdleHG.addFrame(en::Animation::Frame(0, en::Recti(48, 224, 16, 16), en::seconds(0.03f)));
	animHitIdleHG.addFrame(en::Animation::Frame(0, en::Recti(64, 224, 16, 16), en::seconds(0.03f)));
	animHitIdleHG.addFrame(en::Animation::Frame(0, en::Recti(80, 224, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitIdleHG);
	en::Animation animHitIdleHD;
	animHitIdleHD.addFrame(en::Animation::Frame(0, en::Recti(0, 240, 16, 16), en::seconds(0.03f)));
	animHitIdleHD.addFrame(en::Animation::Frame(0, en::Recti(16, 240, 16, 16), en::seconds(0.03f)));
	animHitIdleHD.addFrame(en::Animation::Frame(0, en::Recti(32, 240, 16, 16), en::seconds(0.03f)));
	animHitIdleHD.addFrame(en::Animation::Frame(0, en::Recti(48, 240, 16, 16), en::seconds(0.03f)));
	animHitIdleHD.addFrame(en::Animation::Frame(0, en::Recti(64, 240, 16, 16), en::seconds(0.03f)));
	animHitIdleHD.addFrame(en::Animation::Frame(0, en::Recti(80, 240, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animHitIdleHD);

	// Chop
	en::Animation animChopBG;
	animChopBG.addFrame(en::Animation::Frame(0, en::Recti(0, 256, 16, 16), en::seconds(0.07f)));
	animChopBG.addFrame(en::Animation::Frame(0, en::Recti(16, 256, 16, 16), en::seconds(0.07f)));
	animChopBG.addFrame(en::Animation::Frame(0, en::Recti(32, 256, 16, 16), en::seconds(0.07f)));
	animChopBG.addFrame(en::Animation::Frame(0, en::Recti(48, 256, 16, 16), en::seconds(0.07f)));
	animChopBG.addFrame(en::Animation::Frame(0, en::Recti(64, 256, 16, 16), en::seconds(0.07f)));
	animChopBG.addFrame(en::Animation::Frame(0, en::Recti(80, 256, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChopBG);
	en::Animation animChopBD;
	animChopBD.addFrame(en::Animation::Frame(0, en::Recti(0, 272, 16, 16), en::seconds(0.07f)));
	animChopBD.addFrame(en::Animation::Frame(0, en::Recti(16, 272, 16, 16), en::seconds(0.07f)));
	animChopBD.addFrame(en::Animation::Frame(0, en::Recti(32, 272, 16, 16), en::seconds(0.07f)));
	animChopBD.addFrame(en::Animation::Frame(0, en::Recti(48, 272, 16, 16), en::seconds(0.07f)));
	animChopBD.addFrame(en::Animation::Frame(0, en::Recti(64, 272, 16, 16), en::seconds(0.07f)));
	animChopBD.addFrame(en::Animation::Frame(0, en::Recti(80, 272, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChopBD);
	en::Animation animChopHG;
	animChopHG.addFrame(en::Animation::Frame(0, en::Recti(0, 288, 16, 16), en::seconds(0.07f)));
	animChopHG.addFrame(en::Animation::Frame(0, en::Recti(16, 288, 16, 16), en::seconds(0.07f)));
	animChopHG.addFrame(en::Animation::Frame(0, en::Recti(32, 288, 16, 16), en::seconds(0.07f)));
	animChopHG.addFrame(en::Animation::Frame(0, en::Recti(48, 288, 16, 16), en::seconds(0.07f)));
	animChopHG.addFrame(en::Animation::Frame(0, en::Recti(64, 288, 16, 16), en::seconds(0.07f)));
	animChopHG.addFrame(en::Animation::Frame(0, en::Recti(80, 288, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChopHG);
	en::Animation animChopHD;
	animChopHD.addFrame(en::Animation::Frame(0, en::Recti(0, 304, 16, 16), en::seconds(0.07f)));
	animChopHD.addFrame(en::Animation::Frame(0, en::Recti(16, 304, 16, 16), en::seconds(0.07f)));
	animChopHD.addFrame(en::Animation::Frame(0, en::Recti(32, 304, 16, 16), en::seconds(0.07f)));
	animChopHD.addFrame(en::Animation::Frame(0, en::Recti(48, 304, 16, 16), en::seconds(0.07f)));
	animChopHD.addFrame(en::Animation::Frame(0, en::Recti(64, 304, 16, 16), en::seconds(0.07f)));
	animChopHD.addFrame(en::Animation::Frame(0, en::Recti(80, 304, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChopHD);

	// KO
	en::Animation animKOBG;
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(0, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(16, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(32, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(48, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(64, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(80, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(96, 320, 16, 16), en::seconds(0.03f)));
	animKOBG.addFrame(en::Animation::Frame(0, en::Recti(112, 320, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animKOBG);
	en::Animation animKOBD;
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(0, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(16, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(32, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(48, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(64, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(80, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(96, 336, 16, 16), en::seconds(0.03f)));
	animKOBD.addFrame(en::Animation::Frame(0, en::Recti(112, 336, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animKOBD);
	en::Animation animKOHG;
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(0, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(16, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(32, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(48, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(64, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(80, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(96, 352, 16, 16), en::seconds(0.03f)));
	animKOHG.addFrame(en::Animation::Frame(0, en::Recti(112, 352, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animKOHG);
	en::Animation animKOHD;
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(0, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(16, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(32, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(48, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(64, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(80, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(96, 368, 16, 16), en::seconds(0.03f)));
	animKOHD.addFrame(en::Animation::Frame(0, en::Recti(112, 368, 16, 16), en::seconds(0.03f)));
	mAnimations.push_back(animKOHD);

	// Chopped
	en::Animation animChoppedBG;
	animChoppedBG.addFrame(en::Animation::Frame(0, en::Recti(64, 0, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppedBG);
	en::Animation animChoppedBD;
	animChoppedBD.addFrame(en::Animation::Frame(0, en::Recti(80, 0, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppedBD);
	en::Animation animChoppedHG;
	animChoppedHG.addFrame(en::Animation::Frame(0, en::Recti(96, 0, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppedHG);
	en::Animation animChoppedHD;
	animChoppedHD.addFrame(en::Animation::Frame(0, en::Recti(112, 0, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppedHD);

	// ChoppingWalking
	en::Animation animChoppingWalkingBG;
	animChoppingWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 256, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 256, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 256, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 256, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 256, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBG.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 256, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChoppingWalkingBG);
	en::Animation animChoppingWalkingBD;
	animChoppingWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 272, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 272, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 272, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 272, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 272, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingBD.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 272, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChoppingWalkingBD);
	en::Animation animChoppingWalkingHG;
	animChoppingWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 288, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 288, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 288, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 288, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 288, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHG.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 288, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChoppingWalkingHG);
	en::Animation animChoppingWalkingHD;
	animChoppingWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 0, 304, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 16, 304, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 32, 304, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 48, 304, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 64, 304, 16, 16), en::seconds(0.07f)));
	animChoppingWalkingHD.addFrame(en::Animation::Frame(0, en::Recti(96 + 80, 304, 16, 16), en::seconds(0.07f)));
	mAnimations.push_back(animChoppingWalkingHD);
	
	// ChoppingIdle
	en::Animation animChoppingIdleBG;
	animChoppingIdleBG.addFrame(en::Animation::Frame(0, en::Recti(96, 256, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppingIdleBG);
	en::Animation animChoppingIdleBD;
	animChoppingIdleBD.addFrame(en::Animation::Frame(0, en::Recti(96, 272, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppingIdleBD);
	en::Animation animChoppingIdleHG;
	animChoppingIdleHG.addFrame(en::Animation::Frame(0, en::Recti(96, 288, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppingIdleHG);
	en::Animation animChoppingIdleHD;
	animChoppingIdleHD.addFrame(en::Animation::Frame(0, en::Recti(96, 304, 16, 16), en::seconds(1.0f)));
	mAnimations.push_back(animChoppingIdleHD);

	// Throw
	en::Animation animThrowBG;
	animThrowBG.addFrame(en::Animation::Frame(0, en::Recti(80, 256, 16, 16), en::seconds(0.02f)));
	animThrowBG.addFrame(en::Animation::Frame(0, en::Recti(64, 256, 16, 16), en::seconds(0.02f)));
	animThrowBG.addFrame(en::Animation::Frame(0, en::Recti(48, 256, 16, 16), en::seconds(0.02f)));
	animThrowBG.addFrame(en::Animation::Frame(0, en::Recti(32, 256, 16, 16), en::seconds(0.02f)));
	animThrowBG.addFrame(en::Animation::Frame(0, en::Recti(16, 256, 16, 16), en::seconds(0.02f)));
	animThrowBG.addFrame(en::Animation::Frame(0, en::Recti(0, 256, 16, 16), en::seconds(0.02f)));
	mAnimations.push_back(animThrowBG);
	en::Animation animThrowBD;
	animThrowBD.addFrame(en::Animation::Frame(0, en::Recti(80, 272, 16, 16), en::seconds(0.02f)));
	animThrowBD.addFrame(en::Animation::Frame(0, en::Recti(64, 272, 16, 16), en::seconds(0.02f)));
	animThrowBD.addFrame(en::Animation::Frame(0, en::Recti(48, 272, 16, 16), en::seconds(0.02f)));
	animThrowBD.addFrame(en::Animation::Frame(0, en::Recti(32, 272, 16, 16), en::seconds(0.02f)));
	animThrowBD.addFrame(en::Animation::Frame(0, en::Recti(16, 272, 16, 16), en::seconds(0.02f)));
	animThrowBD.addFrame(en::Animation::Frame(0, en::Recti(0, 272, 16, 16), en::seconds(0.02f)));
	mAnimations.push_back(animThrowBD);
	en::Animation animThrowHG;
	animThrowHG.addFrame(en::Animation::Frame(0, en::Recti(80, 288, 16, 16), en::seconds(0.02f)));
	animThrowHG.addFrame(en::Animation::Frame(0, en::Recti(64, 288, 16, 16), en::seconds(0.02f)));
	animThrowHG.addFrame(en::Animation::Frame(0, en::Recti(48, 288, 16, 16), en::seconds(0.02f)));
	animThrowHG.addFrame(en::Animation::Frame(0, en::Recti(32, 288, 16, 16), en::seconds(0.02f)));
	animThrowHG.addFrame(en::Animation::Frame(0, en::Recti(16, 288, 16, 16), en::seconds(0.02f)));
	animThrowHG.addFrame(en::Animation::Frame(0, en::Recti(0, 288, 16, 16), en::seconds(0.02f)));
	mAnimations.push_back(animThrowHG);
	en::Animation animThrowHD;
	animThrowHD.addFrame(en::Animation::Frame(0, en::Recti(80, 304, 16, 16), en::seconds(0.02f)));
	animThrowHD.addFrame(en::Animation::Frame(0, en::Recti(64, 304, 16, 16), en::seconds(0.02f)));
	animThrowHD.addFrame(en::Animation::Frame(0, en::Recti(48, 304, 16, 16), en::seconds(0.02f)));
	animThrowHD.addFrame(en::Animation::Frame(0, en::Recti(32, 304, 16, 16), en::seconds(0.02f)));
	animThrowHD.addFrame(en::Animation::Frame(0, en::Recti(16, 304, 16, 16), en::seconds(0.02f)));
	animThrowHD.addFrame(en::Animation::Frame(0, en::Recti(0, 304, 16, 16), en::seconds(0.02f)));
	mAnimations.push_back(animThrowHD);
}

void GameSingleton::setCursor(bool enable)
{
	if (enable)
	{
		en::Application::GetInstance().GetWindow().setCursor(en::Window::Cursor::Custom);
		en::Application::GetInstance().GetWindow().setCursorTexture(en::PathManager::GetInstance().GetTexturesPath() + "cursor.png");
		en::Application::GetInstance().GetWindow().setCursorTextureRect(sf::IntRect(0, 0, 32, 32));
		en::Application::GetInstance().GetWindow().setCursorOrigin({ 16,16 });
		en::Application::GetInstance().GetWindow().setCursorScale({ 1.2f,1.2f });
		en::Application::GetInstance().GetWindow().setCursorRotation(45.f);
	}
	else
	{
		en::Application::GetInstance().GetWindow().setCursor(en::Window::Cursor::None);
	}
}

void GameSingleton::clear()
{
	world.reset();
	playerEntity = entt::null;
	nothingEntity = entt::null;

#ifdef ENLIVE_ENABLE_IMGUI
	currentEntity = entt::null;
#endif
}

#ifdef ENLIVE_ENABLE_IMGUI
void GameSingleton::renderMetricsWindow()
{
	if (showWindow)
	{
		ImGui::Begin("Metrics");
		ImGui::Text("Entities : %d", world.size());
		ImGui::Text("FPS : %d", (int)en::Application::GetInstance().GetFPS());
		ImGui::Text("TotalTime : %d s", (int)en::Application::GetInstance().GetTotalDuration().asSeconds());
		ImGui::Text("Mouse X (Window): %d", (int)en::Application::GetInstance().GetWindow().getCursorPosition().x);
		ImGui::Text("Mouse Y (Window): %d", (int)en::Application::GetInstance().GetWindow().getCursorPosition().y);
		ImGui::Text("Mouse X (View): %d", (int)en::Application::GetInstance().GetWindow().getCursorPositionView(GameSingleton::mView).x);
		ImGui::Text("Mouse Y (View): %d", (int)en::Application::GetInstance().GetWindow().getCursorPositionView(GameSingleton::mView).y);
		ImGui::Text("View X : %d", (int)en::Application::GetInstance().GetWindow().getMainView().getCenter().x);
		ImGui::Text("View Y : %d", (int)en::Application::GetInstance().GetWindow().getMainView().getCenter().y);
#ifdef ENLIVE_DEBUG
		ImGui::Text("DebugDisplay Rectangles : %d", en::DebugDraw::getCurrentRectangleCount());
		ImGui::Text("DebugDisplay Circles : %d", en::DebugDraw::getCurrentCircleCount());
#endif // ENLIVE_DEBUG
		ImGui::End();
	}
}
#endif // ENLIVE_ENABLE_IMGUI
