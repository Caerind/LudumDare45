#pragma once

#include <Enlivengine/System/Config.hpp>
#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Math/Random.hpp>
#include <Enlivengine/Core/Components.hpp>

#include <entt/entt.hpp>

#include "GameConfig.hpp"
#include "GameSingleton.hpp"

struct PlayerComponent
{
	bool chopping{ false };

	en::Vector2f getVelocity() const;
};

struct NothingComponent
{
	en::U32 hits{ 0 };
	en::Time koTimer{ en::Time::Zero };
	bool chopped{ false };
	en::Time throwTimer{ en::Time::Zero };

	bool isKO() const { return koTimer > en::Time::Zero; }
	bool isThrowing() const { return throwTimer > en::Time::Zero; }
};

struct AIComponent
{
	en::Time changeRandomMvtTime{ en::seconds(1.2f) };
	en::Time randomMvtTimer{ en::Time::Zero };
	en::RandomEngine randomEngine;

	en::Time cooldownHitTimer{ en::Time::Zero };
};

struct HumanComponent
{
	sf::Sprite body;

	enum class Direction : en::U32
	{
		BasGauche = 0,
		BasDroite = 1,
		HautGauche = 2,
		HautDroite = 3
	};

	Direction direction;

	enum class Animation : en::U32
	{
		Idle = 0,
		Hitted = 1,
		Walking = 2,
		HitWalking = 3,
		HitIdle = 4,
		Chop = 5,
		KO = 6,
		Chopped = 7,
		ChoppingWalking = 8,
		ChoppingIdle = 9,
		Throw = 10,
		Count
	};

	Animation currentAnimation;

	en::F32 life{ 1.0f };

	bool walking{ false };

	en::Time frameTime;
	en::U32 currentFrame;
	bool animLoop;

	void playAnimOnce(Animation animation) { animLoop = false; playAnim(animation); }
	void playAnimLoop(Animation animation) { animLoop = true; playAnim(animation); }
	en::U32 getAnimIdx() const { return ((en::U32)currentAnimation * 4) + (en::U32)direction; }

private:
	void playAnim(Animation anim);
};

struct VelocityComponent
{
	en::Vector2f velocity{ en::Vector2f::zero };
};

struct PropsComponent
{
	sf::Sprite sprite;
	en::Vector2u coords;

	en::U32 gid{ 0 };

	bool destructed{ false };
	en::Time destructTimeAnimation{ en::Time::Zero };
	en::U32 destructCurrentFrame{ 0 };
	en::U32 destructFrames{ 0 };

	static en::Time timePerFrame;

	void destruct();
};

struct DoorComponent
{
	bool enabled{ false };
	en::Time cooldownSpawnTimer{ en::Time::Zero };
};

struct PieceComponent
{
	sf::Sprite sprite;
	en::U32 value;
};