cmake_minimum_required(VERSION 3.13.3)
project(flags)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Enable test cases" ON)

if (BUILD_TESTING)
	enable_testing()
endif()

add_definitions(-DDATAPATH="${CMAKE_CURRENT_LIST_DIR}/data")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory("ext")
add_subdirectory("cpp")
