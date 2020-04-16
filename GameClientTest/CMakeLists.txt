set(SRC_GAMECLIENTTEST
    main.cpp
)
source_group("" FILES ${SRC_GAMECLIENTTEST})

add_executable(GameClientTest ${SRC_GAMECLIENTTEST})
target_link_libraries(GameClientTest PRIVATE Enlivengine)
