set(SRC_GAMESERVERTEST
    main.cpp
)
source_group("" FILES ${SRC_GAMESERVERTEST})

add_executable(GameServerTest ${SRC_GAMESERVERTEST})
target_link_libraries(GameServerTest PRIVATE Enlivengine)
