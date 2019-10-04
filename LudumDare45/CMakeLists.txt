set(PATH_LUDUMDARE45 ${PROJECT_SOURCE_DIR}/LudumDare45)
set(SRC_LUDUMDARE45
    ${PATH_LUDUMDARE45}/main.cpp
)

add_executable(LudumDare45 ${SRC_LUDUMDARE45})
target_link_libraries(LudumDare45 sfml-system OpenGL::GL sfml-window sfml-graphics sfml-network sfml-audio EnlivengineThirdParty EnlivengineLibrary)
