#include "EntityPrefab.hpp"

#include <Enlivengine/Core/Components.hpp>

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