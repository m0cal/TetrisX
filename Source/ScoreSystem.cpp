#include "Headers/ScoreSystem.hpp"
#include <algorithm>

ScoreSystem::ScoreSystem() : 
    current_score(0),
    current_level(1),
    lines_cleared_this_level(0),
    combo_count(0),
    total_tetrominos_placed(0),
    game_start_time(std::chrono::system_clock::now())
{
}

void ScoreSystem::add_line_clear_score(unsigned int lines_cleared, unsigned int total_lines_cleared)
{
    if (lines_cleared == 0)
    {
        reset_combo();
        return;
    }
    
    unsigned int base_score = 0;
    switch (lines_cleared)
    {
        case 1:
            base_score = SINGLE_LINE_BASE_SCORE;
            break;
        case 2:
            base_score = DOUBLE_LINE_BASE_SCORE;
            break;
        case 3:
            base_score = TRIPLE_LINE_BASE_SCORE;
            break;
        case 4:
            base_score = TETRIS_BASE_SCORE;
            break;
        default:
            // For more than 4 lines (if possible)
            base_score = TETRIS_BASE_SCORE + (lines_cleared - 4) * 200;
            break;
    }
    
    // Apply level multiplier
    base_score *= current_level;
    
    // Apply combo bonus
    unsigned int combo_bonus = combo_count * COMBO_MULTIPLIER * current_level;
    
    current_score += base_score + combo_bonus;
    combo_count++;
    
    // Update level based on total lines cleared
    update_level(total_lines_cleared);
}

void ScoreSystem::add_drop_score(bool is_hard_drop, unsigned int cells_dropped)
{
    if (is_hard_drop)
    {
        current_score += cells_dropped * HARD_DROP_SCORE;
    }
    else
    {
        current_score += cells_dropped * SOFT_DROP_SCORE;
    }
}

void ScoreSystem::add_tetromino_placed_score()
{
    total_tetrominos_placed++;
    // Small bonus for placing tetrominos
    current_score += current_level;
}

void ScoreSystem::reset_game()
{
    current_score = 0;
    current_level = 1;
    lines_cleared_this_level = 0;
    combo_count = 0;
    total_tetrominos_placed = 0;
    game_start_time = std::chrono::system_clock::now();
}

void ScoreSystem::update_level(unsigned int total_lines_cleared)
{
    unsigned int new_level = calculate_level_from_lines(total_lines_cleared);
    if (new_level > current_level)
    {
        current_level = new_level;
        lines_cleared_this_level = total_lines_cleared % LINES_PER_LEVEL;
    }
    else
    {
        lines_cleared_this_level = total_lines_cleared % LINES_PER_LEVEL;
    }
}

unsigned int ScoreSystem::calculate_level_from_lines(unsigned int total_lines) const
{
    return std::max(1u, 1 + total_lines / LINES_PER_LEVEL);
}

unsigned int ScoreSystem::get_game_duration_seconds() const
{
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - game_start_time);
    return static_cast<unsigned int>(duration.count());
}

ScoreEntry ScoreSystem::create_score_entry(const std::string& player_name, unsigned int total_lines_cleared) const
{
    return ScoreEntry(
        current_score,
        total_lines_cleared,
        current_level,
        player_name,
        total_tetrominos_placed,
        get_game_duration_seconds()
    );
}
