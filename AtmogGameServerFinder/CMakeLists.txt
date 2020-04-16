set(SRC_ATMOGGAMESERVERFINDER
    main.cpp
)
source_group("" FILES ${SRC_ATMOGGAMESERVERFINDER})

add_executable(AtmogGameServerFinder ${SRC_ATMOGGAMESERVERFINDER})
target_link_libraries(AtmogGameServerFinder PRIVATE Enlivengine)
