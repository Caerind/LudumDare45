cmake_minimum_required(VERSION 3.12...3.15)

project(LudumDare45Project
    VERSION 0.1
    DESCRIPTION "LudumDare45"
    LANGUAGES CXX)
	
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
	set(CMAKE_CXX_EXTENSIONS OFF)
	
	enable_testing()
	
endif()
	
find_package(OpenGL REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

add_subdirectory(Enlivengine)

add_subdirectory(LudumDare45)
