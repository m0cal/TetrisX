#include "Headers/Renderer.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/GetTetromino.hpp"
#include <cmath>

Renderer::Renderer(sf::RenderWindow& window) : window(window), cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1))
{
    // Initialize cell colors
    cell_colors = {
        sf::Color(36, 36, 85),     // Empty/background
        sf::Color(0, 219, 255),    // I-piece (cyan)
        sf::Color(0, 36, 255),     // J-piece (blue)
        sf::Color(255, 146, 0),    // L-piece (orange)
        sf::Color(255, 219, 0),    // O-piece (yellow)
        sf::Color(0, 219, 0),      // S-piece (green)
        sf::Color(146, 0, 255),    // T-piece (purple)
        sf::Color(219, 0, 0),      // Z-piece (red)
        sf::Color(73, 73, 85)      // Ghost/gray
    };
}

void Renderer::render_frame(const GameState& state, const Tetromino& tetromino, unsigned lag)
{
    if (FRAME_DURATION > lag)
    {
        window.clear();
        
        render_matrix(state);
        
        if (!state.is_game_over())
        {
            render_ghost_tetromino(tetromino, state);
        }
        
        render_tetromino(tetromino, state);
        render_line_clear_effect(state);
        render_next_tetromino_preview(state);
        render_ui(state);
        
        window.display();
    }
}

void Renderer::render_matrix(const GameState& state)
{
    const auto& matrix = state.get_matrix();
    const auto& clear_lines = state.get_clear_lines();
    
    for (unsigned char col = 0; col < COLUMNS; col++)
    {
        for (unsigned char row = 0; row < ROWS; row++)
        {
            if (!clear_lines[row])
            {
                cell.setPosition(sf::Vector2f(static_cast<float>(CELL_SIZE * col), static_cast<float>(CELL_SIZE * row)));
                
                if (state.is_game_over() && matrix[col][row] > 0)
                {
                    cell.setFillColor(cell_colors[8]); // Gray for game over
                }
                else
                {
                    cell.setFillColor(cell_colors[matrix[col][row]]);
                }
                
                window.draw(cell);
            }
        }
    }
}

void Renderer::render_ghost_tetromino(const Tetromino& tetromino, const GameState& state)
{
    cell.setFillColor(cell_colors[8]); // Gray color for ghost
    
    for (Position& mino : tetromino.get_ghost_minos(state.get_matrix()))
    {
        cell.setPosition(sf::Vector2f(static_cast<float>(CELL_SIZE * mino.x), static_cast<float>(CELL_SIZE * mino.y)));
        window.draw(cell);
    }
}

void Renderer::render_tetromino(const Tetromino& tetromino, const GameState& state)
{
    if (!state.is_game_over())
    {
        cell.setFillColor(cell_colors[1 + tetromino.get_shape()]);
    }
    
    for (Position& mino : tetromino.get_minos())
    {
        cell.setPosition(sf::Vector2f(static_cast<float>(CELL_SIZE * mino.x), static_cast<float>(CELL_SIZE * mino.y)));
        window.draw(cell);
    }
}

void Renderer::render_line_clear_effect(const GameState& state)
{
    const auto& clear_lines = state.get_clear_lines();
    unsigned char clear_cell_size = calculate_clear_cell_size(state.get_clear_effect_timer());
    
    for (unsigned char col = 0; col < COLUMNS; col++)
    {
        for (unsigned char row = 0; row < ROWS; row++)
        {
            if (clear_lines[row])
            {
                // Draw background
                cell.setFillColor(cell_colors[0]);
                cell.setPosition(sf::Vector2f(static_cast<float>(CELL_SIZE * col), static_cast<float>(CELL_SIZE * row)));
                cell.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                window.draw(cell);
                
                // Draw effect square
                cell.setFillColor(sf::Color(255, 255, 255));
                cell.setPosition(sf::Vector2f(
                    std::floor(CELL_SIZE * (0.5f + col) - 0.5f * clear_cell_size),
                    std::floor(CELL_SIZE * (0.5f + row) - 0.5f * clear_cell_size)
                ));
                cell.setSize(sf::Vector2f(clear_cell_size, clear_cell_size));
                window.draw(cell);
            }
        }
    }
}

void Renderer::render_next_tetromino_preview(const GameState& state)
{
    // Preview border
    sf::RectangleShape preview_border(sf::Vector2f(5 * CELL_SIZE, 5 * CELL_SIZE));
    preview_border.setFillColor(sf::Color(0, 0, 0));
    preview_border.setOutlineThickness(-1);
    preview_border.setPosition(sf::Vector2f(
        CELL_SIZE * (1.5f * COLUMNS - 2.5f),
        CELL_SIZE * (0.25f * ROWS - 2.5f)
    ));
    window.draw(preview_border);
    
    // Next tetromino
    unsigned char next_shape = state.get_next_shape();
    cell.setFillColor(cell_colors[1 + next_shape]);
    cell.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
    
    for (Position& mino : get_tetromino(next_shape, static_cast<unsigned char>(1.5f * COLUMNS), static_cast<unsigned char>(0.25f * ROWS)))
    {
        unsigned short next_tetromino_x = CELL_SIZE * mino.x;
        unsigned short next_tetromino_y = CELL_SIZE * mino.y;
        
        // Adjust position for different shapes
        if (next_shape == 0) // I-piece
        {
            next_tetromino_y += static_cast<unsigned char>(std::round(0.5f * CELL_SIZE));
        }
        else if (next_shape != 3) // Not O-piece
        {
            next_tetromino_x -= static_cast<unsigned char>(std::round(0.5f * CELL_SIZE));
        }
        
        cell.setPosition(sf::Vector2f(next_tetromino_x, next_tetromino_y));
        window.draw(cell);
    }
}

void Renderer::render_ui(const GameState& state)
{
    std::string text = "Lines:" + std::to_string(state.get_lines_cleared()) + 
                      "\nSpeed:" + std::to_string(START_FALL_SPEED / state.get_current_fall_speed()) + 'x';
    
    draw_text(
        static_cast<unsigned short>(CELL_SIZE * (0.5f + COLUMNS)),
        static_cast<unsigned short>(0.5f * CELL_SIZE * ROWS),
        text,
        window
    );
}

unsigned char Renderer::calculate_clear_cell_size(unsigned char clear_effect_timer) const
{
    return static_cast<unsigned char>(2 * std::round(0.5f * CELL_SIZE * (clear_effect_timer / static_cast<float>(CLEAR_EFFECT_DURATION))));
}
