#pragma once

#include <chrono>
#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "InputHandler.hpp"
#include "Renderer.hpp"
#include "LineClearing.hpp"
#include "Tetromino.hpp"

class Game
{
private:
    unsigned lag;
    std::chrono::time_point<std::chrono::steady_clock> previous_time;
    
    GameState state;
    Tetromino tetromino;
    sf::RenderWindow window;
    Renderer renderer;

public:
    Game();
    void run();
    
private:
    void update();
    void handle_tetromino_falling();
    void handle_tetromino_placement();
    void setup_window();
};
