#pragma once

#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>

#include "GameState.hpp"

class MenuState : public en::State
{
	public:
		MenuState(en::StateManager& manager)
			: en::State(manager)
		{
			background.setTexture(getApplication().getResourceManager().Create<en::Texture>("menubg", en::TextureLoader::FromFile("Assets/Textures/background_menu.png")).Get());

			buttonPlay.setTexture(getApplication().getResourceManager().Create<en::Texture>("menuplaybutton", en::TextureLoader::FromFile("Assets/Textures/PlayButtons.png")).Get());
			buttonPlay.setTextureRect(sf::IntRect(0, 0, 96, 32));
			buttonPlay.setPosition(512 - 150, 384);
			buttonPlay.setScale(3.5f, 3.5f);

			/*
			getApplication().getTextures().create("menucontinuebutton", en::TextureLoader::loadFromFile("Assets/Textures/PlayButtons.png"));
			buttonContinue.setTexture(getApplication().getTextures().get("menucontinuebutton"));
			buttonContinue.setTextureRect(sf::IntRect(0, 0, 96, 32));
			buttonContinue.setPosition(512 - 150, 384 + 100);
			buttonContinue.setScale(3.5f, 3.5f);
			*/

			buttonSound.setTexture(getApplication().getResourceManager().Get<en::Texture>("menuplaybutton").Get());
			buttonSound.setTextureRect(sf::IntRect(((GameSingleton::soundEnabled) ? 0 : 18), 114, 14, 14));
			buttonSound.setPosition(512 + 300, 384 + 35);
			buttonSound.setScale(3.5f, 3.5f);
		}

		bool handleEvent(const sf::Event& event) 
		{
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				const sf::Vector2f p((float)event.mouseButton.x, (float)event.mouseButton.y);
				if (buttonPlay.getGlobalBounds().contains(p))
				{
					getApplication().clearStates();
					getApplication().pushState<GameState>();
				}
				/*else if (buttonContinue.getGlobalBounds().contains(p))
				{
					getApplication().clearStates();
					getApplication().pushState<GameState>();
				}*/
				else if (buttonSound.getGlobalBounds().contains(p))
				{
					GameSingleton::soundEnabled = !GameSingleton::soundEnabled;
					if (!GameSingleton::soundEnabled)
					{
						getApplication().getAudio().stop();
					}
				}
			}
			return false; 
		}

		bool update(en::Time dt)
		{
			ENLIVE_UNUSED(dt);

			const sf::Vector2f p = static_cast<sf::Vector2f>(sf::Mouse::getPosition(getApplication().getWindow().getHandle()));
			if (buttonPlay.getGlobalBounds().contains(p))
			{
				buttonPlay.setTextureRect(sf::IntRect(0, 32, 96, 32));
			}
			else
			{
				buttonPlay.setTextureRect(sf::IntRect(0, 0, 96, 32));
			}

			/*
			if (buttonContinue.getGlobalBounds().contains(p))
			{
				buttonContinue.setTextureRect(sf::IntRect(0, 32, 96, 32));
			}
			else
			{
				buttonContinue.setTextureRect(sf::IntRect(0, 0, 96, 32));
			}
			*/

			if (GameSingleton::soundEnabled)
			{
				buttonSound.setTextureRect(sf::IntRect(0, 114, 14, 14));
			}
			else
			{
				buttonSound.setTextureRect(sf::IntRect(18, 114, 14, 14));
			}

			return false;
		}

		void render(sf::RenderTarget& target)
		{
			target.draw(background);
			target.draw(buttonPlay);
			/*
			if (GameSingleton::mFirstIntroDone)
			{
				target.draw(buttonContinue);
			}
			*/
			target.draw(buttonSound);
		}

	private:
		sf::Sprite background;
		sf::Sprite buttonPlay;
		sf::Sprite buttonContinue;
		sf::Sprite buttonSound;
};
