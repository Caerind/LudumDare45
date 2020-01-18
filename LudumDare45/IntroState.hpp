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
			background.setTexture(en::ResourceManager::GetInstance().Create<en::Texture>("atmog", en::TextureLoader::FromFile(en::PathManager::GetInstance().GetTexturesPath() + "atmog.png")).Get());

			accumulator = en::Time::Zero;
			imageIndex = 0;
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
					//getApplication().getTextures().create("storyOfNothing", en::TextureLoader::loadFromFile(en::PathManager::GetInstance().GetTexturesPath() + "storyOfNothing.png"));
					//background.setTexture(getApplication().getTextures().get("storyOfNothing"));
				}
				if (imageIndex == 2)
				{
					getApplication().ClearStates();
					getApplication().PushState<MenuState>();
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
