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
	if (gid != 113)
	{
		GameSingleton::mMap.setCollision(coords, false);
	}

	destructed = true;
	destructCurrentFrame = 1;
}

void HumanComponent::playAnim(Animation anim)
{
	currentAnimation = anim;
	frameTime = en::Time::Zero;
	currentFrame = 0;
	const en::U32 animIdx = getAnimIdx();
	const en::Animation& anima = GameSingleton::mAnimations[animIdx];
	const en::Rectu& rectu = anima.GetFrame(currentFrame).rect;
	const en::Recti recti(static_cast<en::Vector2i>(rectu.getMinimum()), static_cast<en::Vector2i>(rectu.getSize()));
	body.setTextureRect(en::toSF(recti));
}
