#include "GameSingleton.hpp"

#ifdef ENLIVE_ENABLE_IMGUI
#include <imgui/imgui.h>
#endif

#ifdef ENLIVE_DEBUG
#include <Enlivengine/Graphics/DebugDraw.hpp>
#endif

en::Application* GameSingleton::application;

entt::registry GameSingleton::world;

en::ResourceId GameSingleton::mFont;

en::ResourceId GameSingleton::mIndicatorTexture;
en::ResourceId GameSingleton::mTilesetTexture;

en::ResourceId GameSingleton::mAsteroidSound;
en::ResourceId GameSingleton::mExplosionSound;
std::shared_ptr<sf::Sound> GameSingleton::mEngineSoundManager;

en::U32 GameSingleton::mMoney;
en::U32 GameSingleton::mResource1;
en::U32 GameSingleton::mResource2;
en::U32 GameSingleton::mResource3;

#ifdef ENLIVE_ENABLE_IMGUI
entt::entity GameSingleton::currentEntity = entt::null;
ImGuiEntityEditor<entt::registry> GameSingleton::worldEditor;
bool GameSingleton::showWindow = false;
#endif

void GameSingleton::loadResourcesMain(en::Application& application)
{
	GameSingleton::application = &application;

	// Font
	const std::string fontPath = "Assets/Fonts/";
	mFont = application.getFonts().create("sansation", en::FontLoader::loadFromFile(fontPath + "ErasBoldITC.ttf"));
}

void GameSingleton::loadResourcesGame()
{
	// Textures
	const std::string texturePath = "Assets/Textures/";
	mIndicatorTexture = application->getTextures().create("indicator", en::TextureLoader::loadFromFile(texturePath + "indicator.png"));
	application->getTextures().get(mIndicatorTexture).setSmooth(true);
	mTilesetTexture = application->getTextures().create("tileset", en::TextureLoader::loadFromFile(texturePath + "tileset.png"));
	application->getTextures().get(mTilesetTexture).setRepeated(true);
	/*
	mPlanetTexture = application->getTextures().create("planet", oe::TextureLoader::loadFromFile("../Assets/Textures/planet.png"));
	application->getTextures().get(mPlanetTexture).setSmooth(true);
	mShipTexture = application->getTextures().create("ship", oe::TextureLoader::loadFromFile("../Assets/Textures/ship.png"));
	application->getTextures().get(mShipTexture).setSmooth(true);
	mTradeShipTexture = application->getTextures().create("tradeship", oe::TextureLoader::loadFromFile("../Assets/Textures/tradeship.png"));
	application->getTextures().get(mTradeShipTexture).setSmooth(true);
	mProjTexture = application->getTextures().create("projectile", oe::TextureLoader::loadFromFile("../Assets/Textures/projectile.png"));
	application->getTextures().get(mProjTexture).setSmooth(true);
	mEnemy1Texture = application->getTextures().create("enemy-1", oe::TextureLoader::loadFromFile("../Assets/Textures/enemyship-1.png"));
	application->getTextures().get(mEnemy1Texture).setSmooth(true);
	mEnemy2Texture = application->getTextures().create("enemy-2", oe::TextureLoader::loadFromFile("../Assets/Textures/enemyship-2.png"));
	application->getTextures().get(mEnemy2Texture).setSmooth(true);
	mEnemy3Texture = application->getTextures().create("enemy-3", oe::TextureLoader::loadFromFile("../Assets/Textures/enemyship-3.png"));
	application->getTextures().get(mEnemy3Texture).setSmooth(true);
	mHUDTexture = application->getTextures().create("hud", oe::TextureLoader::loadFromFile("../Assets/Textures/hud.png"));
	application->getTextures().get(mHUDTexture).setSmooth(true);
	mParticleTexture = application->getTextures().create("particle", oe::TextureLoader::loadFromFile("../Assets/Textures/particle.png"));
	application->getTextures().get(mParticleTexture).setSmooth(true);
	mAsteroidsTexture = application->getTextures().create("asteroids", oe::TextureLoader::loadFromFile("../Assets/Textures/asteroids.png"));
	application->getTextures().get(mAsteroidsTexture).setSmooth(true);
	mAsteroidsLargeTexture = application->getTextures().create("asteroidslarge", oe::TextureLoader::loadFromFile("../Assets/Textures/asteroids-large.png"));
	application->getTextures().get(mAsteroidsLargeTexture).setSmooth(true);
	mUIBackTexture = application->getTextures().create("ui-back", oe::TextureLoader::loadFromFile("../Assets/Textures/trade-ui-back.png"));
	application->getTextures().get(mUIBackTexture).setSmooth(true);
	mUITopTexture = application->getTextures().create("ui-top", oe::TextureLoader::loadFromFile("../Assets/Textures/trade-ui-top.png"));
	application->getTextures().get(mUITopTexture).setSmooth(true);
	mDeadUITexture = application->getTextures().create("ui-dead", oe::TextureLoader::loadFromFile("../Assets/Textures/dead-ui.png"));
	application->getTextures().get(mDeadUITexture).setSmooth(true);
	mMenuUITexture = application->getTextures().create("ui-menu", oe::TextureLoader::loadFromFile("../Assets/Textures/menu-ui.png"));
	application->getTextures().get(mMenuUITexture).setSmooth(true);
	*/

	// Sound
	const std::string soundPath = "Assets/Textures/";
	mAsteroidSound = application->getAudio().createSound("asteroid", soundPath + "asteroid.wav");
	mExplosionSound = application->getAudio().createSound("explosion", soundPath + "explosion.wav");
	/*
	mShieldSound = application->getAudio().createSound("shield", "../Assets/Sounds/shield.wav");
	mShoot1Sound = application->getAudio().createSound("shoot-1", "../Assets/Sounds/shoot-1.wav");
	mShoot2Sound = application->getAudio().createSound("shoot-2", "../Assets/Sounds/shoot-2.wav");
	mClickSound = application->getAudio().createSound("click", "../Assets/Sounds/click.wav");
	*/
}

void GameSingleton::clear()
{
	//mMoney = GameConfig::getU("money-start");
	mMoney = 0;
	mResource1 = 0;
	mResource2 = 0;
	mResource3 = 0;

	/*
	mPlayerBoost = GameConfig::getF("player-boost");
	mPlayerLife = GameConfig::getI("player-life");
	mPlayerArmor = GameConfig::getI("player-armor");

	mTraderCapacity = GameConfig::getU("trader-capacity");
	mTraderLife = GameConfig::getI("trader-life");
	mTraderArmor = GameConfig::getI("trader-armor");
	*/
}

void GameSingleton::playSound(en::ResourceId r, const en::Vector2f& position)
{
	bool play = true;
	/*
	if (r != mShoot1Sound)
	{
		if ((mPlayerShip->getPosition() - position).getLength() > 1200.0f)
		{
			play = false;
		}
	}
	*/
	if (play)
	{
		application->getAudio().playSound(r)->setVolume(20.0f);
	}
}

void GameSingleton::playClick()
{
	//application->getAudio().playSound(mClickSound)->setVolume(20.0f);
}

#ifdef ENLIVE_ENABLE_IMGUI
void GameSingleton::renderMetricsWindow()
{
	if (showWindow)
	{
		ImGui::Begin("Metrics");
		ImGui::Text("Entities : %d", world.size());
		ImGui::Text("FPS : %d", (int)application->getFPS());
		ImGui::Text("TotalTime : %d s", (int)application->getTotalDuration().asSeconds());
		const en::View winView(0, 0, 1024, 768);
		ImGui::Text("Mouse X (Window): %d", (int)application->getWindow().getCursorPositionView(winView).x);
		ImGui::Text("Mouse Y (Window): %d", (int)application->getWindow().getCursorPositionView(winView).y);
		ImGui::Text("Mouse X (View): %d", (int)application->getWindow().getCursorPosition().x);
		ImGui::Text("Mouse Y (View): %d", (int)application->getWindow().getCursorPosition().y);
		ImGui::Text("View X : %d", (int)application->getWindow().getMainView().getCenter().x);
		ImGui::Text("View Y : %d", (int)application->getWindow().getMainView().getCenter().y);
#ifdef ENLIVE_DEBUG
		ImGui::Text("DebugDisplay Rectangles : %d", en::DebugDraw::getCurrentRectangleCount());
		ImGui::Text("DebugDisplay Circles : %d", en::DebugDraw::getCurrentCircleCount());
#endif // ENLIVE_DEBUG
		ImGui::End();
	}
}
#endif // ENLIVE_ENABLE_IMGUI
