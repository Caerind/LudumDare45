#pragma once

#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>
#include <Enlivengine/Graphics/View.hpp>

#include <entt/entt.hpp>

#include "GameConfig.hpp"
#include "GameSingleton.hpp"

class GameState : public en::State
{
	public:
		GameState(en::StateManager& manager);

		bool handleEvent(const sf::Event& event);
		bool update(en::Time dt);
		void render(sf::RenderTarget& target);

	private:
		inline en::Window& getWindow();
		en::Vector2f getAdjustedCursorPos();

		bool isCollision(const en::Vector2f& point);

		bool isCollisionSlow(const en::Vector2f& point);
		bool isCollisionFast(const en::Vector2f& point);

		void velocities();
		void directions();
		void movements(en::Time dt);

#ifdef ENLIVE_DEBUG
		void devEvent(const sf::Event& event);
		void devUpdate(en::Time& dt);
#endif // ENLIVE_DEBUG

	private:
		en::Time mCursorTime;
		en::U32 mCursorFrame;
		sf::Sprite mCursor;
};
