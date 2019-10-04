set(PATH_LUDUMDARE45 ${PROJECT_SOURCE_DIR}/LudumDare45)
set(SRC_LUDUMDARE45
	${PATH_LUDUMDARE45}/EditorComponents.hpp
    ${PATH_LUDUMDARE45}/EntityPrefab.hpp
    ${PATH_LUDUMDARE45}/EntityPrefab.cpp
    ${PATH_LUDUMDARE45}/GameConfig.hpp
    ${PATH_LUDUMDARE45}/GameConfig.cpp
    ${PATH_LUDUMDARE45}/GameSingleton.hpp
    ${PATH_LUDUMDARE45}/GameSingleton.cpp
    ${PATH_LUDUMDARE45}/GameState.hpp
    ${PATH_LUDUMDARE45}/GameState.cpp
    ${PATH_LUDUMDARE45}/imgui_entt_entity_editor.hpp
    ${PATH_LUDUMDARE45}/main.cpp
)
source_group("" FILES ${SRC_LUDUMDARE45})

add_executable(LudumDare45 ${SRC_LUDUMDARE45})
target_link_libraries(LudumDare45 sfml-system OpenGL::GL sfml-window sfml-graphics sfml-network sfml-audio EnlivengineThirdParty EnlivengineLibrary)
