#pragma once

#include <Enlivengine/System/Time.hpp>
#include <Enlivengine/Math/Random.hpp>

#include <SFML/Graphics.hpp>

#include <unordered_map>

class NothingTextSystem
{
	public:
		NothingTextSystem(sf::Texture& texture, sf::Font& font)
		{
			mRandom.setSeed(87645);

			sf::Vector2f scale{ 5.3334f, 4.f };

			mBackground.setTexture(texture);
			mBackground.setScale(scale);

			en::F32 height = mBackground.getGlobalBounds().height;

			mBackground.setPosition(sf::Vector2f(0.f, 768 - height));

			mText.setFont(font);
			mText.setPosition(sf::Vector2f(32.f, 768.f - height * 0.6));
			mText.setScale(sf::Vector2f{ 1.f, 1.f });
			mText.setString("");
			mText.setCharacterSize(25);
			mText.setFillColor(sf::Color::White);
			mText.setOutlineColor(sf::Color::Black);
			mText.setOutlineThickness(1.0f);

			mDisplaying = false;
			mDuration = en::seconds(6.0f);
			mCurrentEvent = Event::None;
		}

		enum class Event
		{
			None = 0,
			Random = 1,
			Tutorial1 = 2,
			Tutorial2 = 3,
			Tutorial3 = 4,
			FightBegin = 5
		};

		void handleEvent(Event event)
		{
			if (!mDisplaying || mCurrentEvent < event)
			{
				play(event);
			}
		}

		void update(en::Time dt)
		{
			if (mDisplaying)
			{
				mDuration -= dt;
				if (mDuration < en::Time::Zero)
				{
					mDisplaying = false;
					mDuration = en::seconds(6.0f);
				}
			}
			else
			{
				mDuration -= dt;
				if (mDuration < en::Time::Zero)
				{
					play(Event::Random);
				}
			}
		}

		void render(sf::RenderTarget& target)
		{
			if (mDisplaying)
			{
				target.draw(mBackground);
				target.draw(mText);
			}
		}

	private:
		void play(Event event)
		{
			mCurrentEvent = event;
			mDisplaying = true;
			mDuration = en::seconds(4.0f);

			mText.setFillColor(sf::Color::White);

			switch (event)
			{
			case Event::None: {
				mDisplaying = false;
				mDuration = en::seconds(2.0f);
				return;
			}
			case Event::Random: {
				mDuration = en::seconds(3.0f);
				en::U32 i = mRandom.get<en::U32>(0, 5);
				switch (i)
				{
				case 0: mText.setString("Nothing  :  the  weather  is  nice"); break;
				case 1: mText.setString("Nothing  :  i'm  hungry  today"); break;
				case 2: mText.setString("Nothing  :  have  you  ever  heard  about  'NalRokh'?"); break;
				case 3: mText.setString("Nothing  :  nice  day  for  fishing  ain't  it?"); break;
				case 4: mText.setString("Nothing  :  this morning i ate an asparagus"); break;
				}
				if (mText.getString().getSize() == 0)
				{
					mDisplaying = false;
					mDuration = en::seconds(2.0f);
				}
				return;
			}
			case Event::Tutorial1: {
				mDuration = en::seconds(6.0f);
				mText.setString("Use  'WASD'  or  'ZQSD'  to  move  around");
				mText.setFillColor(sf::Color::Green);
				return;
			}
			case Event::Tutorial2: {
				mDuration = en::seconds(6.0f);
				mText.setString("Use  'Left'  click  to  hit.  (That  'Nothing'  guy  is  boring  right?)");
				mText.setFillColor(sf::Color::Green);
				return;
			}
			case Event::Tutorial3: {
				mDuration = en::seconds(6.0f);
				mText.setString("When  he  is  KO,  use  'Right'  click  to  get  him");
				mText.setFillColor(sf::Color::Green);
				return;
			}
			case Event::FightBegin: {
				mDuration = en::seconds(4.0f);
				mText.setString("???  :  MWHAHAHA!!  You  are  both  going  to  die!!");
				mText.setFillColor(sf::Color::Red);
				return;
			}
			}
		}

	private:
		en::RandomEngine mRandom;
		sf::Sprite mBackground;
		sf::Text mText;
		en::Time mDuration;
		bool mDisplaying;
		Event mCurrentEvent;
};
