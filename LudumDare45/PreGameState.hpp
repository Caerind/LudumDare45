#pragma once

#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>

#include "GameState.hpp"

class PreGameState : public en::State
{
	public:
		PreGameState(en::StateManager& manager)
			: en::State(manager)
		{
			background.setTexture(en::ResourceManager::GetInstance().Create<en::Texture>("atmog", en::SFMLResourcesLoader<en::Texture>::FromFile(en::PathManager::GetInstance().GetTexturesPath() + "atmog.png")).Get());

			accumulator = en::Time::Zero;
			imageIndex = 0;
		}

		bool update(en::Time dt)
		{
			accumulator += dt;
			if (accumulator >= en::seconds(0.1f))
			{
				accumulator = en::Time::Zero;
				imageIndex++;

				if (imageIndex == 1)
				{

				}
				if (imageIndex == 2)
				{
					GameSingleton::mFirstIntroDone = true;
					getApplication().clearStates();
					getApplication().pushState<GameState>();
				}
			}
			return false;
		}

		void render(sf::RenderTarget& target)
		{
			target.draw(background);
		}

	private:
		sf::Sprite background;
		en::Time accumulator;
		en::U32 imageIndex;
};
