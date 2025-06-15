#include "Headers/GameState.hpp"
#include <algorithm>

GameState::GameState() : 
    game_over(false),
    hard_drop_pressed(false),
    rotate_pressed(false),
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
    shape_distribution(0, 6)
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
    
    // Increase speed every certain number of lines
    if (0 == lines_cleared % LINES_TO_INCREASE_SPEED)
    {
        current_fall_speed = std::max<unsigned char>(SOFT_DROP_SPEED, current_fall_speed - 1);
    }
}

unsigned char GameState::generate_next_shape()
{
    return static_cast<unsigned char>(shape_distribution(random_engine));
}

void GameState::clear_clear_lines()
{
    std::fill(clear_lines.begin(), clear_lines.end(), false);
}
