#include "EntityPrefab.hpp"

#include <Enlivengine/Core/Components.hpp>

#include "Components.hpp"

bool EntityPrefab::createEntity(entt::registry& world, en::F32 x, en::F32 y)
{
	auto entity = GameSingleton::world.create();
	auto& position = GameSingleton::world.assign<en::PositionComponent>(entity);
	position.setPosition(x, y);
	auto& renderable = GameSingleton::world.assign<en::RenderableComponent>(entity);
	renderable.z = 0;
	auto& sprite = GameSingleton::world.assign<en::SpriteComponent>(entity);
	//sprite.setTexture(getApplication().getTextures().get(asteroidTextureId));
	sprite.setTextureRect(en::toSF(en::Recti(0, 0, 120, 120)));
	sprite.setOrigin(60, 60);
	auto& text = GameSingleton::world.assign<en::TextComponent>(entity);
	//text.setFont(getApplication().getFonts().get(fontId));
	text.setPosition(10, 10);
	text.setString("Asteroid");
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.f);

	return true;
}

bool EntityPrefab::createPlayer(entt::registry& world, entt::entity& playerEntity)
{
	playerEntity = world.create();
	auto& nameComponent = world.assign<en::NameComponent>(playerEntity);
	nameComponent.insert(0, "Player");
	auto& positionComponent = world.assign<en::PositionComponent>(playerEntity);
	auto& renderableComponent = world.assign<en::RenderableComponent>(playerEntity);
	auto& playerComponent = world.assign<PlayerComponent>(playerEntity);
	auto& humanComponent = world.assign<HumanComponent>(playerEntity);
	humanComponent.body.setTexture(GameSingleton::application->getTextures().get(GameSingleton::mEverythingTexture));
	humanComponent.body.setOrigin(8, 16);
	humanComponent.body.setTextureRect(sf::IntRect(0, 0, 16, 16));
	auto& velocityComponent = world.assign<VelocityComponent>(playerEntity);
	return true;
}

bool EntityPrefab::createNothing(entt::registry& world, entt::entity& nothingEntity)
{
	nothingEntity = world.create();
	auto& nameComponent = world.assign<en::NameComponent>(nothingEntity);
	nameComponent.insert(0, "Nothing");
	auto& positionComponent = world.assign<en::PositionComponent>(nothingEntity);
	auto& renderableComponent = world.assign<en::RenderableComponent>(nothingEntity);
	auto& nothingComponent = world.assign<NothingComponent>(nothingEntity);
	auto& humanComponent = world.assign<HumanComponent>(nothingEntity);
	humanComponent.body.setTexture(GameSingleton::application->getTextures().get(GameSingleton::mNothingTexture));
	humanComponent.body.setOrigin(8, 16);
	humanComponent.body.setTextureRect(sf::IntRect(0, 0, 16, 16));
	humanComponent.direction = HumanComponent::Direction::BasGauche;
	humanComponent.currentAnimation = HumanComponent::Animation::Idle;
	humanComponent.animLoop = true;
	humanComponent.frameTime = en::Time::Zero;
	humanComponent.currentFrame = 0;
	auto& velocityComponent = world.assign<VelocityComponent>(nothingEntity);
	return true;
}


bool EntityPrefab::createProps(entt::registry& world, en::F32 x, en::F32 y, en::U32 gid, en::Tileset& tileset)
{
	static int propsCounter = 0;
	auto entity = world.create();
	auto& nameComponent = world.assign<en::NameComponent>(entity);
	nameComponent.insert(0, "Props-" + std::to_string(propsCounter++));
	auto& positionComponent = world.assign<en::PositionComponent>(entity);
	positionComponent.setPosition(x + 8, y + 8);
	auto& renderableComponent = world.assign<en::RenderableComponent>(entity);
	auto& propsComponent = world.assign<PropsComponent>(entity);
	propsComponent.sprite.setTexture(tileset.getTexture());
	propsComponent.sprite.setOrigin(8, 8);
	sf::Vector2f vertexPos = tileset.toPos(gid);
	propsComponent.sprite.setTextureRect(sf::IntRect((int)vertexPos.x, (int)vertexPos.y, 16, 16));
	propsComponent.coords = GameSingleton::mMap.worldToCoords(en::Vector2f(x + 8, y + 8));
	propsComponent.destructFrames = 3;
	propsComponent.gid = gid;

	if (gid == 113)
	{
		auto& doorComponent = world.assign<DoorComponent>(entity);
		doorComponent.cooldownSpawnTimer = en::seconds(en::Random::get<en::I32>(8,20));
	}

	return true;
}

bool EntityPrefab::createAI(entt::registry& world, en::F32 x, en::F32 y)
{
	if (GameSingleton::world.view<AIComponent>().size() > 50)
	{
		return true;
	}

	static int aiCounter = 0;
	auto entity = world.create();
	auto& nameComponent = world.assign<en::NameComponent>(entity);
	nameComponent.insert(0, "AI-" + std::to_string(aiCounter++));
	auto& positionComponent = world.assign<en::PositionComponent>(entity);
	positionComponent.setPosition(x, y);
	auto& renderableComponent = world.assign<en::RenderableComponent>(entity);
	auto& aiComponent = world.assign<AIComponent>(entity);
	aiComponent.randomEngine.setSeed(aiCounter);
	auto& humanComponent = world.assign<HumanComponent>(entity);
	humanComponent.body.setTexture(GameSingleton::application->getTextures().get(GameSingleton::mAITexture));
	humanComponent.body.setOrigin(8, 16);
	humanComponent.body.setTextureRect(sf::IntRect(0, 0, 16, 16));
	humanComponent.direction = HumanComponent::Direction::BasGauche;
	humanComponent.currentAnimation = HumanComponent::Animation::Idle;
	humanComponent.animLoop = true;
	humanComponent.frameTime = en::Time::Zero;
	humanComponent.currentFrame = 0;
	auto& velocityComponent = world.assign<VelocityComponent>(entity);
	return true;
}
