cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)

project(TEST)

find_package(Catch2 3 REQUIRED)

add_compile_options(--coverage -g)

add_library(game ../lib/include/game.hpp ../lib/src/game.cpp)

add_library(SchoolsTable ../lib/include/SchoolsTable.hpp ../lib/src/SchoolsTable.cpp)

add_library(units ../lib/include/units.hpp ../lib/src/units.cpp)

add_link_options(--coverage)

link_libraries(game units SchoolsTable)

add_executable(test test.cpp)

target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
