#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "Tetromino.hpp"

class InputHandler
{
public:
    static void handle_events(sf::RenderWindow& window, GameState& state);
    static void handle_gameplay_input(GameState& state, Tetromino& tetromino);
    static void handle_menu_input(GameState& state);
    static void handle_menu_mouse_input(sf::RenderWindow& window, GameState& state);
    
private:
    static void handle_rotation_input(GameState& state, Tetromino& tetromino);
    static void handle_movement_input(GameState& state, Tetromino& tetromino);
    static void handle_drop_input(GameState& state, Tetromino& tetromino);
};
