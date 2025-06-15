#pragma once

#include <vector>
#include "Global.hpp"
#include "GameState.hpp"

class LineClearing
{
public:
    static void check_and_mark_lines_for_clearing(GameState& state);
    static void process_line_clearing_effect(GameState& state);
    
private:
    static bool is_line_full(const std::vector<std::vector<unsigned char>>& matrix, unsigned char row);
    static void clear_marked_lines(GameState& state);
};
