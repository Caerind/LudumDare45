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
};
