#pragma once

#include <vector>
#include <string>
#include <chrono>
#include "Global.hpp"

struct ScoreEntry
{
    unsigned long score;
    unsigned int lines_cleared;
    unsigned int level;
    std::string player_name;
    std::chrono::system_clock::time_point timestamp;
    unsigned int tetrominos_placed;
    unsigned int game_duration_seconds;
    
    ScoreEntry(unsigned long s = 0, unsigned int lines = 0, unsigned int lvl = 1, 
               const std::string& name = "Player", unsigned int tetrominos = 0, unsigned int duration = 0)
        : score(s), lines_cleared(lines), level(lvl), player_name(name), 
          tetrominos_placed(tetrominos), game_duration_seconds(duration),
          timestamp(std::chrono::system_clock::now()) {}
};

class ScoreSystem
{
private:
    unsigned long current_score;
    unsigned int current_level;
    unsigned int lines_cleared_this_level;
    unsigned int combo_count;
    unsigned int total_tetrominos_placed;
    std::chrono::system_clock::time_point game_start_time;
    
    // Scoring constants
    static const unsigned int SINGLE_LINE_BASE_SCORE = 100;
    static const unsigned int DOUBLE_LINE_BASE_SCORE = 300;
    static const unsigned int TRIPLE_LINE_BASE_SCORE = 500;
    static const unsigned int TETRIS_BASE_SCORE = 800;
    static const unsigned int SOFT_DROP_SCORE = 1;
    static const unsigned int HARD_DROP_SCORE = 2;
    static const unsigned int COMBO_MULTIPLIER = 50;
    static const unsigned int LINES_PER_LEVEL = 10;

public:
    ScoreSystem();
    
    // Getters
    unsigned long get_current_score() const { return current_score; }
    unsigned int get_current_level() const { return current_level; }
    unsigned int get_combo_count() const { return combo_count; }
    unsigned int get_total_tetrominos_placed() const { return total_tetrominos_placed; }
    unsigned int get_lines_cleared_this_level() const { return lines_cleared_this_level; }
    unsigned int get_game_duration_seconds() const;
    
    // Score calculation methods
    void add_line_clear_score(unsigned int lines_cleared, unsigned int total_lines_cleared);
    void add_drop_score(bool is_hard_drop, unsigned int cells_dropped = 1);
    void add_tetromino_placed_score();
    void reset_combo() { combo_count = 0; }
    void reset_game();
    
    // Create score entry for current game
    ScoreEntry create_score_entry(const std::string& player_name, unsigned int total_lines_cleared) const;
    
private:
    void update_level(unsigned int total_lines_cleared);
    unsigned int calculate_level_from_lines(unsigned int total_lines) const;
};
