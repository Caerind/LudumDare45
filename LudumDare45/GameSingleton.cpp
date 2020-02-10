#include "GameSingleton.hpp"

#ifdef ENLIVE_ENABLE_IMGUI
#include <imgui/imgui.h>
#endif

#ifdef ENLIVE_DEBUG
#include <Enlivengine/Graphics/DebugDraw.hpp>
#endif

#include <Enlivengine/Graphics/SFMLResources.hpp>

#include "Components.hpp"

en::Application* GameSingleton::application;

bool GameSingleton::mFirstIntroDone = false;
bool GameSingleton::mFirstThrowNothingDone = false;

GameMap GameSingleton::mMap;
en::View GameSingleton::mView;

entt::registry GameSingleton::world;
entt::entity GameSingleton::playerEntity;
entt::entity GameSingleton::nothingEntity;

en::FontPtr GameSingleton::mFont;

en::AnimationPtr GameSingleton::mNothingAnim;
en::AnimationPtr GameSingleton::mEverythingAnim;
en::AnimationPtr GameSingleton::mAIAnim;
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

en::Animation GameSingleton::mAnimations[44];

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
    mNothingAnim = en::ResourceManager::GetInstance().Get<en::Animation>("anim_perso_nothing");
    if (mNothingAnim.IsValid())
    {
        mNothingTexture = mNothingAnim.Get().GetTexture();
    }
    mEverythingAnim = en::ResourceManager::GetInstance().Get<en::Animation>("anim_perso_everything");
    if (mEverythingAnim.IsValid())
    {
        mEverythingTexture = mEverythingAnim.Get().GetTexture();
    }
    mAIAnim = en::ResourceManager::GetInstance().Get<en::Animation>("anim_perso_IA");
    if (mAIAnim.IsValid())
    {
        mAITexture = mAIAnim.Get().GetTexture();
    }

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
	mAnimations[0].AddFrame(en::Rectu(0, 0, 16, 16), en::milliseconds(300));
	mAnimations[0].AddFrame(en::Rectu(16, 0, 16, 16), en::milliseconds(300));
	mAnimations[0].AddFrame(en::Rectu(32, 0, 16, 16), en::milliseconds(300));
	mAnimations[1].AddFrame(en::Rectu(0, 16, 16, 16), en::milliseconds(300));
	mAnimations[1].AddFrame(en::Rectu(16, 16, 16, 16), en::milliseconds(300));
	mAnimations[1].AddFrame(en::Rectu(32, 16, 16, 16), en::milliseconds(300));
	mAnimations[2].AddFrame(en::Rectu(0, 32, 16, 16), en::milliseconds(300));
	mAnimations[2].AddFrame(en::Rectu(16, 32, 16, 16), en::milliseconds(300));
	mAnimations[2].AddFrame(en::Rectu(32, 32, 16, 16), en::milliseconds(300));
	mAnimations[3].AddFrame(en::Rectu(0, 48, 16, 16), en::milliseconds(300));
	mAnimations[3].AddFrame(en::Rectu(16, 48, 16, 16), en::milliseconds(300));
	mAnimations[3].AddFrame(en::Rectu(32, 48, 16, 16), en::milliseconds(300));

	// Hitted
	mAnimations[4].AddFrame(en::Rectu(0, 64, 16, 16), en::milliseconds(30));
	mAnimations[4].AddFrame(en::Rectu(16, 64, 16, 16), en::milliseconds(30));
	mAnimations[4].AddFrame(en::Rectu(32, 64, 16, 16), en::milliseconds(30));
	mAnimations[4].AddFrame(en::Rectu(48, 64, 16, 16), en::milliseconds(30));
	mAnimations[5].AddFrame(en::Rectu(0, 80, 16, 16), en::milliseconds(30));
	mAnimations[5].AddFrame(en::Rectu(16, 80, 16, 16), en::milliseconds(30));
	mAnimations[5].AddFrame(en::Rectu(32, 80, 16, 16), en::milliseconds(30));
	mAnimations[5].AddFrame(en::Rectu(48, 80, 16, 16), en::milliseconds(30));
	mAnimations[6].AddFrame(en::Rectu(0, 96, 16, 16), en::milliseconds(30));
	mAnimations[6].AddFrame(en::Rectu(16, 96, 16, 16), en::milliseconds(30));
	mAnimations[6].AddFrame(en::Rectu(32, 96, 16, 16), en::milliseconds(30));
	mAnimations[6].AddFrame(en::Rectu(48, 96, 16, 16), en::milliseconds(30));
	mAnimations[7].AddFrame(en::Rectu(0, 112, 16, 16), en::milliseconds(30));
	mAnimations[7].AddFrame(en::Rectu(16, 112, 16, 16), en::milliseconds(30));
	mAnimations[7].AddFrame(en::Rectu(32, 112, 16, 16), en::milliseconds(30));
	mAnimations[7].AddFrame(en::Rectu(48, 112, 16, 16), en::milliseconds(30));

	// Walking
	mAnimations[8].AddFrame(en::Rectu(0, 128, 16, 16), en::milliseconds(100));
	mAnimations[8].AddFrame(en::Rectu(16, 128, 16, 16), en::milliseconds(100));
	mAnimations[8].AddFrame(en::Rectu(32, 128, 16, 16), en::milliseconds(100));
	mAnimations[8].AddFrame(en::Rectu(48, 128, 16, 16), en::milliseconds(100));
	mAnimations[8].AddFrame(en::Rectu(64, 128, 16, 16), en::milliseconds(100));
	mAnimations[8].AddFrame(en::Rectu(80, 128, 16, 16), en::milliseconds(100));
	mAnimations[9].AddFrame(en::Rectu(0, 144, 16, 16), en::milliseconds(100));
	mAnimations[9].AddFrame(en::Rectu(16, 144, 16, 16), en::milliseconds(100));
	mAnimations[9].AddFrame(en::Rectu(32, 144, 16, 16), en::milliseconds(100));
	mAnimations[9].AddFrame(en::Rectu(48, 144, 16, 16), en::milliseconds(100));
	mAnimations[9].AddFrame(en::Rectu(64, 144, 16, 16), en::milliseconds(100));
	mAnimations[9].AddFrame(en::Rectu(80, 144, 16, 16), en::milliseconds(100));
	mAnimations[10].AddFrame(en::Rectu(0, 160, 16, 16), en::milliseconds(100));
	mAnimations[10].AddFrame(en::Rectu(16, 160, 16, 16), en::milliseconds(100));
	mAnimations[10].AddFrame(en::Rectu(32, 160, 16, 16), en::milliseconds(100));
	mAnimations[10].AddFrame(en::Rectu(48, 160, 16, 16), en::milliseconds(100));
	mAnimations[10].AddFrame(en::Rectu(64, 160, 16, 16), en::milliseconds(100));
	mAnimations[10].AddFrame(en::Rectu(80, 160, 16, 16), en::milliseconds(100));
	mAnimations[11].AddFrame(en::Rectu(0, 176, 16, 16), en::milliseconds(100));
	mAnimations[11].AddFrame(en::Rectu(16, 176, 16, 16), en::milliseconds(100));
	mAnimations[11].AddFrame(en::Rectu(32, 176, 16, 16), en::milliseconds(100));
	mAnimations[11].AddFrame(en::Rectu(48, 176, 16, 16), en::milliseconds(100));
	mAnimations[11].AddFrame(en::Rectu(64, 176, 16, 16), en::milliseconds(100));
	mAnimations[11].AddFrame(en::Rectu(80, 176, 16, 16), en::milliseconds(100));

	// HitWalking
	mAnimations[12].AddFrame(en::Rectu(96 + 0, 192, 16, 16), en::milliseconds(30));
	mAnimations[12].AddFrame(en::Rectu(96 + 16, 192, 16, 16), en::milliseconds(30));
	mAnimations[12].AddFrame(en::Rectu(96 + 32, 192, 16, 16), en::milliseconds(30));
	mAnimations[12].AddFrame(en::Rectu(96 + 48, 192, 16, 16), en::milliseconds(30));
	mAnimations[12].AddFrame(en::Rectu(96 + 64, 192, 16, 16), en::milliseconds(30));
	mAnimations[12].AddFrame(en::Rectu(96 + 80, 192, 16, 16), en::milliseconds(30));
	mAnimations[13].AddFrame(en::Rectu(96 + 0, 208, 16, 16), en::milliseconds(30));
	mAnimations[13].AddFrame(en::Rectu(96 + 16, 208, 16, 16), en::milliseconds(30));
	mAnimations[13].AddFrame(en::Rectu(96 + 32, 208, 16, 16), en::milliseconds(30));
	mAnimations[13].AddFrame(en::Rectu(96 + 48, 208, 16, 16), en::milliseconds(30));
	mAnimations[13].AddFrame(en::Rectu(96 + 64, 208, 16, 16), en::milliseconds(30));
	mAnimations[13].AddFrame(en::Rectu(96 + 80, 208, 16, 16), en::milliseconds(30));
	mAnimations[14].AddFrame(en::Rectu(96 + 0, 224, 16, 16), en::milliseconds(30));
	mAnimations[14].AddFrame(en::Rectu(96 + 16, 224, 16, 16), en::milliseconds(30));
	mAnimations[14].AddFrame(en::Rectu(96 + 32, 224, 16, 16), en::milliseconds(30));
	mAnimations[14].AddFrame(en::Rectu(96 + 48, 224, 16, 16), en::milliseconds(30));
	mAnimations[14].AddFrame(en::Rectu(96 + 64, 224, 16, 16), en::milliseconds(30));
	mAnimations[14].AddFrame(en::Rectu(96 + 80, 224, 16, 16), en::milliseconds(30));
	mAnimations[15].AddFrame(en::Rectu(96 + 0, 240, 16, 16), en::milliseconds(30));
	mAnimations[15].AddFrame(en::Rectu(96 + 16, 240, 16, 16), en::milliseconds(30));
	mAnimations[15].AddFrame(en::Rectu(96 + 32, 240, 16, 16), en::milliseconds(30));
	mAnimations[15].AddFrame(en::Rectu(96 + 48, 240, 16, 16), en::milliseconds(30));
	mAnimations[15].AddFrame(en::Rectu(96 + 64, 240, 16, 16), en::milliseconds(30));
	mAnimations[15].AddFrame(en::Rectu(96 + 80, 240, 16, 16), en::milliseconds(30));

	// HitIdle
	mAnimations[16].AddFrame(en::Rectu(0, 192, 16, 16), en::milliseconds(30));
	mAnimations[16].AddFrame(en::Rectu(16, 192, 16, 16), en::milliseconds(30));
	mAnimations[16].AddFrame(en::Rectu(32, 192, 16, 16), en::milliseconds(30));
	mAnimations[16].AddFrame(en::Rectu(48, 192, 16, 16), en::milliseconds(30));
	mAnimations[16].AddFrame(en::Rectu(64, 192, 16, 16), en::milliseconds(30));
	mAnimations[16].AddFrame(en::Rectu(80, 192, 16, 16), en::milliseconds(30));
	mAnimations[17].AddFrame(en::Rectu(0, 208, 16, 16), en::milliseconds(30));
	mAnimations[17].AddFrame(en::Rectu(16, 208, 16, 16), en::milliseconds(30));
	mAnimations[17].AddFrame(en::Rectu(32, 208, 16, 16), en::milliseconds(30));
	mAnimations[17].AddFrame(en::Rectu(48, 208, 16, 16), en::milliseconds(30));
	mAnimations[17].AddFrame(en::Rectu(64, 208, 16, 16), en::milliseconds(30));
	mAnimations[17].AddFrame(en::Rectu(80, 208, 16, 16), en::milliseconds(30));
	mAnimations[18].AddFrame(en::Rectu(0, 224, 16, 16), en::milliseconds(30));
	mAnimations[18].AddFrame(en::Rectu(16, 224, 16, 16), en::milliseconds(30));
	mAnimations[18].AddFrame(en::Rectu(32, 224, 16, 16), en::milliseconds(30));
	mAnimations[18].AddFrame(en::Rectu(48, 224, 16, 16), en::milliseconds(30));
	mAnimations[18].AddFrame(en::Rectu(64, 224, 16, 16), en::milliseconds(30));
	mAnimations[18].AddFrame(en::Rectu(80, 224, 16, 16), en::milliseconds(30));
	mAnimations[19].AddFrame(en::Rectu(0, 240, 16, 16), en::milliseconds(30));
	mAnimations[19].AddFrame(en::Rectu(16, 240, 16, 16), en::milliseconds(30));
	mAnimations[19].AddFrame(en::Rectu(32, 240, 16, 16), en::milliseconds(30));
	mAnimations[19].AddFrame(en::Rectu(48, 240, 16, 16), en::milliseconds(30));
	mAnimations[19].AddFrame(en::Rectu(64, 240, 16, 16), en::milliseconds(30));
	mAnimations[19].AddFrame(en::Rectu(80, 240, 16, 16), en::milliseconds(30));

	// Chop
	mAnimations[20].AddFrame(en::Rectu(0, 256, 16, 16), en::milliseconds(70));
	mAnimations[20].AddFrame(en::Rectu(16, 256, 16, 16), en::milliseconds(70));
	mAnimations[20].AddFrame(en::Rectu(32, 256, 16, 16), en::milliseconds(70));
	mAnimations[20].AddFrame(en::Rectu(48, 256, 16, 16), en::milliseconds(70));
	mAnimations[20].AddFrame(en::Rectu(64, 256, 16, 16), en::milliseconds(70));
	mAnimations[20].AddFrame(en::Rectu(80, 256, 16, 16), en::milliseconds(70));
	mAnimations[21].AddFrame(en::Rectu(0, 272, 16, 16), en::milliseconds(70));
	mAnimations[21].AddFrame(en::Rectu(16, 272, 16, 16), en::milliseconds(70));
	mAnimations[21].AddFrame(en::Rectu(32, 272, 16, 16), en::milliseconds(70));
	mAnimations[21].AddFrame(en::Rectu(48, 272, 16, 16), en::milliseconds(70));
	mAnimations[21].AddFrame(en::Rectu(64, 272, 16, 16), en::milliseconds(70));
	mAnimations[21].AddFrame(en::Rectu(80, 272, 16, 16), en::milliseconds(70));
	mAnimations[22].AddFrame(en::Rectu(0, 288, 16, 16), en::milliseconds(70));
	mAnimations[22].AddFrame(en::Rectu(16, 288, 16, 16), en::milliseconds(70));
	mAnimations[22].AddFrame(en::Rectu(32, 288, 16, 16), en::milliseconds(70));
	mAnimations[22].AddFrame(en::Rectu(48, 288, 16, 16), en::milliseconds(70));
	mAnimations[22].AddFrame(en::Rectu(64, 288, 16, 16), en::milliseconds(70));
	mAnimations[22].AddFrame(en::Rectu(80, 288, 16, 16), en::milliseconds(70));
	mAnimations[23].AddFrame(en::Rectu(0, 304, 16, 16), en::milliseconds(70));
	mAnimations[23].AddFrame(en::Rectu(16, 304, 16, 16), en::milliseconds(70));
	mAnimations[23].AddFrame(en::Rectu(32, 304, 16, 16), en::milliseconds(70));
	mAnimations[23].AddFrame(en::Rectu(48, 304, 16, 16), en::milliseconds(70));
	mAnimations[23].AddFrame(en::Rectu(64, 304, 16, 16), en::milliseconds(70));
	mAnimations[23].AddFrame(en::Rectu(80, 304, 16, 16), en::milliseconds(70));

	// KO
	mAnimations[24].AddFrame(en::Rectu(0, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(16, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(32, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(48, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(64, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(80, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(96, 320, 16, 16), en::milliseconds(30));
	mAnimations[24].AddFrame(en::Rectu(112, 320, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(0, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(16, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(32, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(48, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(64, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(80, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(96, 336, 16, 16), en::milliseconds(30));
	mAnimations[25].AddFrame(en::Rectu(112, 336, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(0, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(16, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(32, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(48, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(64, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(80, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(96, 352, 16, 16), en::milliseconds(30));
	mAnimations[26].AddFrame(en::Rectu(112, 352, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(0, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(16, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(32, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(48, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(64, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(80, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(96, 368, 16, 16), en::milliseconds(30));
	mAnimations[27].AddFrame(en::Rectu(112, 368, 16, 16), en::milliseconds(30));

	// Chopped
	mAnimations[28].AddFrame(en::Rectu(64, 0, 16, 16), en::milliseconds(1000));
	mAnimations[29].AddFrame(en::Rectu(80, 0, 16, 16), en::milliseconds(1000));
	mAnimations[30].AddFrame(en::Rectu(96, 0, 16, 16), en::milliseconds(1000));
	mAnimations[31].AddFrame(en::Rectu(112, 0, 16, 16), en::milliseconds(1000));

	// ChoppingWalking
	mAnimations[32].AddFrame(en::Rectu(96 + 0, 256, 16, 16), en::milliseconds(70));
	mAnimations[32].AddFrame(en::Rectu(96 + 16, 256, 16, 16), en::milliseconds(70));
	mAnimations[32].AddFrame(en::Rectu(96 + 32, 256, 16, 16), en::milliseconds(70));
	mAnimations[32].AddFrame(en::Rectu(96 + 48, 256, 16, 16), en::milliseconds(70));
	mAnimations[32].AddFrame(en::Rectu(96 + 64, 256, 16, 16), en::milliseconds(70));
	mAnimations[32].AddFrame(en::Rectu(96 + 80, 256, 16, 16), en::milliseconds(70));
	mAnimations[33].AddFrame(en::Rectu(96 + 0, 272, 16, 16), en::milliseconds(70));
	mAnimations[33].AddFrame(en::Rectu(96 + 16, 272, 16, 16), en::milliseconds(70));
	mAnimations[33].AddFrame(en::Rectu(96 + 32, 272, 16, 16), en::milliseconds(70));
	mAnimations[33].AddFrame(en::Rectu(96 + 48, 272, 16, 16), en::milliseconds(70));
	mAnimations[33].AddFrame(en::Rectu(96 + 64, 272, 16, 16), en::milliseconds(70));
	mAnimations[33].AddFrame(en::Rectu(96 + 80, 272, 16, 16), en::milliseconds(70));
	mAnimations[34].AddFrame(en::Rectu(96 + 0, 288, 16, 16), en::milliseconds(70));
	mAnimations[34].AddFrame(en::Rectu(96 + 16, 288, 16, 16), en::milliseconds(70));
	mAnimations[34].AddFrame(en::Rectu(96 + 32, 288, 16, 16), en::milliseconds(70));
	mAnimations[34].AddFrame(en::Rectu(96 + 48, 288, 16, 16), en::milliseconds(70));
	mAnimations[34].AddFrame(en::Rectu(96 + 64, 288, 16, 16), en::milliseconds(70));
	mAnimations[34].AddFrame(en::Rectu(96 + 80, 288, 16, 16), en::milliseconds(70));
	mAnimations[35].AddFrame(en::Rectu(96 + 0, 304, 16, 16), en::milliseconds(70));
	mAnimations[35].AddFrame(en::Rectu(96 + 16, 304, 16, 16), en::milliseconds(70));
	mAnimations[35].AddFrame(en::Rectu(96 + 32, 304, 16, 16), en::milliseconds(70));
	mAnimations[35].AddFrame(en::Rectu(96 + 48, 304, 16, 16), en::milliseconds(70));
	mAnimations[35].AddFrame(en::Rectu(96 + 64, 304, 16, 16), en::milliseconds(70));
	mAnimations[35].AddFrame(en::Rectu(96 + 80, 304, 16, 16), en::milliseconds(70));
	
	// ChoppingIdle
	mAnimations[36].AddFrame(en::Rectu(96, 256, 16, 16), en::milliseconds(1000));
	mAnimations[37].AddFrame(en::Rectu(96, 272, 16, 16), en::milliseconds(1000));
	mAnimations[38].AddFrame(en::Rectu(96, 288, 16, 16), en::milliseconds(1000));
	mAnimations[39].AddFrame(en::Rectu(96, 304, 16, 16), en::milliseconds(1000));

	// Throw
	mAnimations[40].AddFrame(en::Rectu(80, 256, 16, 16), en::milliseconds(20));
	mAnimations[40].AddFrame(en::Rectu(64, 256, 16, 16), en::milliseconds(20));
	mAnimations[40].AddFrame(en::Rectu(48, 256, 16, 16), en::milliseconds(20));
	mAnimations[40].AddFrame(en::Rectu(32, 256, 16, 16), en::milliseconds(20));
	mAnimations[40].AddFrame(en::Rectu(16, 256, 16, 16), en::milliseconds(20));
	mAnimations[40].AddFrame(en::Rectu(0, 256, 16, 16), en::milliseconds(20));
	mAnimations[41].AddFrame(en::Rectu(80, 272, 16, 16), en::milliseconds(20));
	mAnimations[41].AddFrame(en::Rectu(64, 272, 16, 16), en::milliseconds(20));
	mAnimations[41].AddFrame(en::Rectu(48, 272, 16, 16), en::milliseconds(20));
	mAnimations[41].AddFrame(en::Rectu(32, 272, 16, 16), en::milliseconds(20));
	mAnimations[41].AddFrame(en::Rectu(16, 272, 16, 16), en::milliseconds(20));
	mAnimations[41].AddFrame(en::Rectu(0, 272, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(80, 288, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(64, 288, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(48, 288, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(32, 288, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(16, 288, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(0, 288, 16, 16), en::milliseconds(20));
	mAnimations[42].AddFrame(en::Rectu(80, 304, 16, 16), en::milliseconds(20));
	mAnimations[43].AddFrame(en::Rectu(64, 304, 16, 16), en::milliseconds(20));
	mAnimations[43].AddFrame(en::Rectu(48, 304, 16, 16), en::milliseconds(20));
	mAnimations[43].AddFrame(en::Rectu(32, 304, 16, 16), en::milliseconds(20));
	mAnimations[43].AddFrame(en::Rectu(16, 304, 16, 16), en::milliseconds(20));
	mAnimations[43].AddFrame(en::Rectu(0, 304, 16, 16), en::milliseconds(20));
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
