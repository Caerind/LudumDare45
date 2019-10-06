#include "Components.hpp"

en::Time PropsComponent::timePerFrame = en::seconds(0.01f);

en::Vector2f PlayerComponent::getVelocity() const 
{
	en::Vector2f playerVelocity(en::Vector2f::zero);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		playerVelocity.y -= 1.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		playerVelocity.y += 1.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		playerVelocity.x -= 1.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		playerVelocity.x += 1.0f;
	}
	if (playerVelocity.getSquaredLength() > 0.5f)
	{
		return playerVelocity.normalized();
	}
	else
	{
		return en::Vector2f::zero;
	}
}

void PropsComponent::destruct()
{
	GameSingleton::mMap.setCollision(coords, false);

	destructed = true;
	destructCurrentFrame = 1;
}
