#pragma once

#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>

#include "MenuState.hpp"

class IntroState : public en::State
{
	public:
		IntroState(en::StateManager& manager)
			: en::State(manager)
		{
			getApplication().getTextures().create("atmog", en::TextureLoader::loadFromFile("Assets/Textures/atmog.png"));
			background.setTexture(getApplication().getTextures().get("atmog"));

			accumulator = en::Time::Zero;
			imageIndex = 0;
		}

		bool handleEvent(const sf::Event& event)
		{
			if (event.type == sf::Event::Closed)
			{
				getApplication().stop();
			}
			return false;
		}

		bool update(en::Time dt)
		{
			accumulator += dt;
			if (accumulator >= en::seconds(1.0f))
			{
				accumulator = en::Time::Zero;
				imageIndex++;

				if (imageIndex == 1)
				{
					//getApplication().getTextures().create("storyOfNothing", en::TextureLoader::loadFromFile("Assets/Textures/storyOfNothing.png"));
					//background.setTexture(getApplication().getTextures().get("storyOfNothing"));
				}
				if (imageIndex == 2)
				{
					getApplication().clearStates();
					getApplication().pushState<MenuState>();
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
