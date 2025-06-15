#pragma once

#include <chrono>
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "GameState.hpp"
#include "InputHandler.hpp"
#include "Renderer.hpp"
#include "LineClearing.hpp"
#include "Tetromino.hpp"
#include "AudioManager.hpp"

class Game
{
private:
    unsigned lag;
    std::chrono::time_point<std::chrono::steady_clock> previous_time;
    bool failure_sound_played;
    GameMode previous_game_mode;
    
    GameState state;
    Tetromino tetromino;
    sf::RenderWindow window;
    Renderer renderer;
    AudioManager audio_manager;

public:
    Game();
    void run();
    AudioManager& get_audio_manager() { return audio_manager; }
    void reset_failure_sound_flag() { failure_sound_played = false; }
    
private:
    void update();
    void handle_tetromino_falling();
    void handle_tetromino_placement();
    void setup_window();
};
