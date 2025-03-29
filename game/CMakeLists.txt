cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)

project(SUMMONERS)

add_compile_options(-g)

add_library(manager ../lib/include/GameManager.hpp ../lib/src/GameManager.cpp)

add_library(viewer ../lib/include/GameView.hpp ../lib/src/GameView.cpp)

add_library(game ../lib/include/game.hpp ../lib/src/game.cpp)

add_library(SchoolsTable ../lib/include/SchoolsTable.hpp ../lib/src/SchoolsTable.cpp)

add_library(units ../lib/include/units.hpp ../lib/src/units.cpp)

link_libraries(game manager viewer units SchoolsTable)

add_executable(summoners summoners.cpp)

