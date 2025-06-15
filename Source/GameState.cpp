#include "Headers/GameState.hpp"
#include <algorithm>

GameState::GameState() : 
    game_over(false),
    hard_drop_pressed(false),
    rotate_pressed(false),
    exit_requested(false),
    current_mode(GameMode::MENU),
    selected_menu_option(0),
    lines_cleared(0),
    clear_effect_timer(0),
    current_fall_speed(START_FALL_SPEED),
    fall_timer(0),
    move_timer(0),
    soft_drop_timer(0),
    clear_lines(ROWS, false),
    matrix(COLUMNS, std::vector<unsigned char>(ROWS, 0)),
    random_engine(random_device()),
    shape_distribution(0, 6),
    score_system()
{
    next_shape = generate_next_shape();
}

void GameState::reset_game()
{
    game_over = false;
    hard_drop_pressed = false;
    rotate_pressed = false;
    current_mode = GameMode::PLAYING;
    selected_menu_option = 0;
    lines_cleared = 0;
    current_fall_speed = START_FALL_SPEED;
    fall_timer = 0;
    move_timer = 0;
    soft_drop_timer = 0;
    
    // Reset score system
    score_system.reset_game();
    
    // Clear the matrix
    for (std::vector<unsigned char>& column : matrix)
    {
        std::fill(column.begin(), column.end(), 0);
    }
}

void GameState::increment_lines_cleared()
{
    lines_cleared++;
    clear_effect_timer = CLEAR_EFFECT_DURATION;
    
    // Update fall speed based on score system level
    update_fall_speed_from_level();
}

void GameState::increment_lines_cleared(unsigned int count)
{
    lines_cleared += count;
    clear_effect_timer = CLEAR_EFFECT_DURATION;
    
    // Update fall speed based on score system level
    update_fall_speed_from_level();
}

void GameState::update_fall_speed_from_level()
{
    // Calculate fall speed based on level from score system
    unsigned int level = score_system.get_current_level();
    current_fall_speed = std::max<unsigned char>(
        SOFT_DROP_SPEED, 
        START_FALL_SPEED - static_cast<unsigned char>((level - 1) * 2)
    );
}

unsigned char GameState::generate_next_shape()
{
    return static_cast<unsigned char>(shape_distribution(random_engine));
}

void GameState::clear_clear_lines()
{
    std::fill(clear_lines.begin(), clear_lines.end(), false);
}
