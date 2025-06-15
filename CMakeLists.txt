cmake_minimum_required(VERSION 3.16)
project(tetris CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

add_executable(tetris
    Source/AudioManager.cpp
    Source/DrawText.cpp
    Source/GetTetromino.cpp
    Source/GetWallKickData.cpp
    Source/Main.cpp
    Source/Tetromino.cpp
    Source/GameState.cpp
    Source/InputHandler.cpp
    Source/LineClearing.cpp
    Source/Renderer.cpp
    Source/Game.cpp
    Source/ScoreSystem.cpp
    Source/LeaderboardManager.cpp)
target_include_directories(tetris PRIVATE Source/Headers)
target_link_libraries(tetris PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
install(TARGETS tetris)
