set(SRC_LUDUMDARE45
	Components.hpp
	Components.cpp
	EditorComponents.hpp
    EntityPrefab.hpp
    EntityPrefab.cpp
    GameConfig.hpp
    GameConfig.cpp
    GameMap.hpp
    GameMap.cpp
    GameSingleton.hpp
    GameSingleton.cpp
    GameState.hpp
    GameState.cpp
    IntroState.hpp
    MenuState.hpp
    PreGameState.hpp
    NothingTextSystem.hpp
    imgui_entt_entity_editor.hpp
    main.cpp
)
source_group("" FILES ${SRC_LUDUMDARE45})

add_executable(LudumDare45 ${SRC_LUDUMDARE45})
target_link_libraries(LudumDare45 PRIVATE Enlivengine)
