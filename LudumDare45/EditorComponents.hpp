#pragma once

#include <Enlivengine/System/Config.hpp>

#ifdef ENLIVE_ENABLE_IMGUI

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Core/Components.hpp>

#include <entt/entt.hpp>
#include "imgui_entt_entity_editor.hpp"

#include "GameConfig.hpp"
#include "GameSingleton.hpp"

class EditorComponents
{
	public:
		static void registerToEditor(ImGuiEntityEditor<entt::registry>& editor)
		{
			#define TrivialComponent(ComponentType, ComponentName) \
				editor.registerTrivial<##ComponentType>(GameSingleton::world, #ComponentName); \
				editor.registerComponentWidgetFn( \
					GameSingleton::world.type<##ComponentType>(), \
					[](entt::registry& reg, auto e) { \
						auto& comp = reg.get<##ComponentType>(e); \
						EditorComponents::##ComponentName(comp); });

			TrivialComponent(en::PositionComponent, Position);
			TrivialComponent(en::RenderableComponent, Renderable);



			#undef TrivialComponent
		}

		static void Position(en::PositionComponent& position)
		{
			bool positionChanged = false;
			float x = position.getPosition().x;
			float y = position.getPosition().y;
			positionChanged |= ImGui::SliderFloat("x##Position", &x, -1000.f, +1000.f);
			positionChanged |= ImGui::SliderFloat("y##Position", &y, -1000.f, +1000.f);
			if (positionChanged)
			{
				position.setPosition(x, y);
			}

			bool rotationChanged = false;
			float angle = position.getRotation();
			rotationChanged |= ImGui::SliderFloat("angle##Position", &angle, 0.0f, 360.0f);
			if (rotationChanged)
			{
				position.setRotation(angle);
			}
		}

		static void Renderable(en::RenderableComponent& renderable)
		{
			bool positionChanged = false;
			int z = (int)renderable.z;
			positionChanged |= ImGui::SliderInt("z##Renderable", &z, 0, 100);
			if (positionChanged)
			{
				renderable.z = z;
			}
		}
};

#endif // ENLIVE_ENABLE_IMGUI