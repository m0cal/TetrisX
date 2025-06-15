#include "Headers/LineClearing.hpp"
#include "Headers/AudioManager.hpp"

void LineClearing::check_and_mark_lines_for_clearing(GameState& state, AudioManager& audio_manager)
{
    const auto& matrix = state.get_matrix();
    bool lines_found = false;
    
    for (unsigned char row = 0; row < ROWS; row++)
    {
        if (is_line_full(matrix, row))
        {
            state.increment_lines_cleared();
            state.set_clear_line(row, true);
            lines_found = true;
        }
    }
    
    // Play clear sound if any lines were found
    if (lines_found)
    {
        audio_manager.play_sound(SoundType::CLEAR);
    }
}

void LineClearing::process_line_clearing_effect(GameState& state)
{
    state.decrement_clear_effect_timer();
    
    if (state.get_clear_effect_timer() == 0)
    {
        clear_marked_lines(state);
        state.clear_clear_lines();
    }
}

bool LineClearing::is_line_full(const std::vector<std::vector<unsigned char>>& matrix, unsigned char row)
{
    for (unsigned char col = 0; col < COLUMNS; col++)
    {
        if (matrix[col][row] == 0)
        {
            return false;
        }
    }
    return true;
}

void LineClearing::clear_marked_lines(GameState& state)
{
    auto& matrix = state.get_matrix();
    const auto& clear_lines = state.get_clear_lines();
    
    for (unsigned char row = 0; row < ROWS; row++)
    {
        if (clear_lines[row])
        {
            for (unsigned char col = 0; col < COLUMNS; col++)
            {
                matrix[col][row] = 0;
                
                // Move all rows above down
                for (unsigned char moveRow = row; moveRow > 0; moveRow--)
                {
                    matrix[col][moveRow] = matrix[col][moveRow - 1];
                    matrix[col][moveRow - 1] = 0;
                }
            }
        }
    }
}
