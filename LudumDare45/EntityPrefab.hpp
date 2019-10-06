#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Log.hpp>

#include <entt/entt.hpp>
#include "imgui_entt_entity_editor.hpp"

#include "GameConfig.hpp"
#include "GameSingleton.hpp"

class EntityPrefab
{
	public:
		static bool createEntity(entt::registry& world, en::F32 x, en::F32 y);

		static bool createPlayer(entt::registry& world, entt::entity& playerEntity);
		static bool createNothing(entt::registry& world, entt::entity& nothingEntt);
		static bool createProps(entt::registry& world, en::F32 x, en::F32 y, en::U32 gid, en::Tileset& tileset);
		static bool createAI(entt::registry& world, en::F32 x, en::F32 y);
};
