#pragma once

#include <vector>
#include <random>
#include "Global.hpp"
#include "ScoreSystem.hpp"

class GameState
{
private:
    bool game_over;
    bool hard_drop_pressed;
    bool rotate_pressed;
    bool exit_requested;
    
    GameMode current_mode;
    int selected_menu_option; // 0 = Start Game, 1 = Leaderboard, 2 = Exit
    
    unsigned lines_cleared;
    unsigned char clear_effect_timer;
    unsigned char current_fall_speed;
    unsigned char fall_timer;
    unsigned char move_timer;
    unsigned char next_shape;
    unsigned char soft_drop_timer;
    
    std::vector<bool> clear_lines;
    std::vector<std::vector<unsigned char>> matrix;
    
    std::random_device random_device;
    std::default_random_engine random_engine;
    std::uniform_int_distribution<unsigned short> shape_distribution;
    
    ScoreSystem score_system;

public:
    GameState();
    
    // Getters
    bool is_game_over() const { return game_over; }
    bool is_hard_drop_pressed() const { return hard_drop_pressed; }
    bool is_rotate_pressed() const { return rotate_pressed; }
    bool is_exit_requested() const { return exit_requested; }
    GameMode get_current_mode() const { return current_mode; }
    int get_selected_menu_option() const { return selected_menu_option; }
    unsigned get_lines_cleared() const { return lines_cleared; }
    unsigned char get_clear_effect_timer() const { return clear_effect_timer; }
    unsigned char get_current_fall_speed() const { return current_fall_speed; }
    unsigned char get_fall_timer() const { return fall_timer; }
    unsigned char get_move_timer() const { return move_timer; }
    unsigned char get_next_shape() const { return next_shape; }
    unsigned char get_soft_drop_timer() const { return soft_drop_timer; }
    const std::vector<bool>& get_clear_lines() const { return clear_lines; }
    const std::vector<std::vector<unsigned char>>& get_matrix() const { return matrix; }
    std::vector<std::vector<unsigned char>>& get_matrix() { return matrix; }
    ScoreSystem& get_score_system() { return score_system; }
    const ScoreSystem& get_score_system() const { return score_system; }
    
    // Setters
    void set_game_over(bool value) { game_over = value; }
    void set_hard_drop_pressed(bool value) { hard_drop_pressed = value; }
    void set_rotate_pressed(bool value) { rotate_pressed = value; }
    void set_exit_requested(bool value) { exit_requested = value; }
    void set_current_mode(GameMode mode) { current_mode = mode; }
    void set_selected_menu_option(int option) { selected_menu_option = option; }
    void set_fall_timer(unsigned char value) { fall_timer = value; }
    void set_move_timer(unsigned char value) { move_timer = value; }
    void set_soft_drop_timer(unsigned char value) { soft_drop_timer = value; }
    void set_clear_effect_timer(unsigned char value) { clear_effect_timer = value; }
    
    // Game logic methods
    void reset_game();
    void increment_fall_timer() { fall_timer++; }
    void increment_lines_cleared();
    void increment_lines_cleared(unsigned int count);
    void decrement_clear_effect_timer() { clear_effect_timer--; }
    void update_move_timer() { move_timer = (1 + move_timer) % MOVE_SPEED; }
    void update_soft_drop_timer() { soft_drop_timer = (1 + soft_drop_timer) % SOFT_DROP_SPEED; }
    unsigned char generate_next_shape();
    void set_next_shape(unsigned char shape) { next_shape = shape; }
    void set_clear_line(unsigned char row, bool value) { clear_lines[row] = value; }
    void clear_clear_lines();
    void update_fall_speed_from_level();
};
