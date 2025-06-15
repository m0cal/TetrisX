#include "Headers/Renderer.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/GetTetromino.hpp"
#include <cmath>
#include <algorithm>

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
    
    // Load background texture
    if (background_texture.loadFromFile("Resources/Images/background.jpg"))
    {
        background_sprite = std::make_unique<sf::Sprite>(background_texture);
        
        // Scale the background to fit the window
        sf::Vector2u texture_size = background_texture.getSize();
        sf::Vector2u window_size = window.getSize();
        
        float scale_x = static_cast<float>(window_size.x) / texture_size.x;
        float scale_y = static_cast<float>(window_size.y) / texture_size.y;
        
        background_sprite->setScale(sf::Vector2f(scale_x, scale_y));
    }
}

void Renderer::render_frame(const GameState& state, const Tetromino& tetromino, unsigned lag, const LeaderboardManager* leaderboard_manager)
{
    if (FRAME_DURATION > lag)
    {
        window.clear();
        
        // Draw background first
        if (background_sprite)
        {
            window.draw(*background_sprite);
        }
        
        if (state.get_current_mode() == GameMode::MENU)
        {
            render_menu(state);
        }
        else if (state.get_current_mode() == GameMode::LEADERBOARD)
        {
            if (leaderboard_manager)
            {
                render_leaderboard(*leaderboard_manager);
            }
        }
        else if (state.get_current_mode() == GameMode::GAME_OVER || state.is_game_over())
        {
            // Game over: show background and game over text only
            render_ui(state);
        }
        else
        {
            render_matrix(state);
            
            if (!state.is_game_over())
            {
                render_ghost_tetromino(tetromino, state);
            }
            
            render_tetromino(tetromino, state);
            render_line_clear_effect(state);
            render_next_tetromino_preview(state);
            render_ui(state);
        }
        
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
    if (state.is_game_over())
    {
        // Game over: only show game over text
        // Calculate center position for game over text
        unsigned short center_x = static_cast<unsigned short>(CELL_SIZE * COLUMNS);
        unsigned short center_y = static_cast<unsigned short>(CELL_SIZE * ROWS * 0.4f);
        
        // Draw "GAME OVER" with smaller font size
        draw_text_centered(
            center_x,
            center_y,
            "GAME OVER",
            window,
            sf::Color::Red,
            1.2f  // Smaller scale compared to default
        );
        
        // Draw instruction text with even smaller font
        draw_text_centered(
            center_x,
            static_cast<unsigned short>(center_y + CELL_SIZE * 3),
            "Press ENTER to return to menu",
            window,
            sf::Color::White,
            0.6f  // Smaller scale for instruction text
        );
    }
    else
    {
        // Normal game: show statistics and score info
        render_score_info(state);
    }
}

void Renderer::render_score_info(const GameState& state)
{
    const ScoreSystem& score_system = state.get_score_system();
    
    std::string score_text = "Score: " + std::to_string(score_system.get_current_score());
    std::string level_text = "Level: " + std::to_string(score_system.get_current_level());
    std::string lines_text = "Lines: " + std::to_string(state.get_lines_cleared());
    std::string combo_text = "Combo: " + std::to_string(score_system.get_combo_count());
    
    // Position on the right side of the game area
    unsigned short text_x = static_cast<unsigned short>(CELL_SIZE * (0.5f + COLUMNS));
    unsigned short start_y = static_cast<unsigned short>(0.5f * CELL_SIZE * ROWS);
    
    draw_text(text_x, start_y, score_text, window, sf::Color::White);
    draw_text(text_x, static_cast<unsigned short>(start_y + CELL_SIZE * 2), level_text, window, sf::Color::Cyan);
    draw_text(text_x, static_cast<unsigned short>(start_y + CELL_SIZE * 4), lines_text, window, sf::Color::Green);
    
    if (score_system.get_combo_count() > 0)
    {
        draw_text(text_x, static_cast<unsigned short>(start_y + CELL_SIZE * 6), combo_text, window, sf::Color::Yellow);
    }
}

unsigned char Renderer::calculate_clear_cell_size(unsigned char clear_effect_timer) const
{
    return static_cast<unsigned char>(2 * std::round(0.5f * CELL_SIZE * (clear_effect_timer / static_cast<float>(CLEAR_EFFECT_DURATION))));
}

void Renderer::render_menu(const GameState& state)
{
    // Calculate center positions
    unsigned short center_x = static_cast<unsigned short>(CELL_SIZE * COLUMNS);
    
    // Draw title (larger)
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.15f),
        "TETRIS X",
        window,
        sf::Color::Cyan,
        2.0f
    );
    
    // Draw menu options (smaller)
    sf::Color start_color = (state.get_selected_menu_option() == 0) ? sf::Color::Yellow : sf::Color::White;
    sf::Color leaderboard_color = (state.get_selected_menu_option() == 1) ? sf::Color::Yellow : sf::Color::White;
    sf::Color exit_color = (state.get_selected_menu_option() == 2) ? sf::Color::Yellow : sf::Color::White;
    
    // Draw START option
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.4f),
        "START GAME",
        window,
        start_color,
        0.8f
    );
    
    // Draw LEADERBOARD option
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.5f),
        "LEADERBOARD",
        window,
        leaderboard_color,
        0.8f
    );
    
    // Draw EXIT option
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.6f),
        "EXIT",
        window,
        exit_color,
        0.8f
    );
    
    // Draw selection indicator (left side of selected option)
    unsigned short indicator_y = static_cast<unsigned short>(CELL_SIZE * ROWS * (0.4f + 0.1f * state.get_selected_menu_option()));
    unsigned short start_text_width = get_text_width("START GAME", 0.8f);
    unsigned short leaderboard_text_width = get_text_width("LEADERBOARD", 0.8f);
    unsigned short exit_text_width = get_text_width("EXIT", 0.8f);
    unsigned short max_text_width = std::max({start_text_width, leaderboard_text_width, exit_text_width});
    
    draw_text_centered(
        static_cast<unsigned short>(center_x - max_text_width / 2 - CELL_SIZE),
        indicator_y,
        ">",
        window,
        sf::Color::Green,
        0.8f
    );
    
    // Draw instructions (smaller)
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.75f),
        "Use mouse or UP/DOWN keys to navigate",
        window,
        sf::Color::Magenta,
        0.6f
    );
    
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.8f),
        "Click or press ENTER to select",
        window,
        sf::Color::Magenta,
        0.6f
    );
}

void Renderer::render_leaderboard(const LeaderboardManager& leaderboard_manager)
{
    window.clear();
    
    // Draw background
    if (background_sprite)
    {
        window.draw(*background_sprite);
    }
    
    unsigned short center_x = static_cast<unsigned short>(CELL_SIZE * COLUMNS);
    
    // Draw title
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.1f),
        "HIGH SCORES",
        window,
        sf::Color::Cyan,
        1.5f
    );
    
    // Draw header
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.2f),
        "Rank  Name      Score     Level Lines Time",
        window,
        sf::Color::White,
        0.6f
    );
    
    // Draw scores
    const auto& high_scores = leaderboard_manager.get_high_scores();
    for (size_t i = 0; i < high_scores.size() && i < 8; ++i)
    {
        std::string entry_text = leaderboard_manager.format_score_entry(high_scores[i], i + 1);
        
        draw_text_centered(
            center_x,
            static_cast<unsigned short>(CELL_SIZE * ROWS * (0.3f + i * 0.06f)),
            entry_text,
            window,
            sf::Color::White,
            0.5f
        );
    }
    
    // If no scores yet
    if (high_scores.empty())
    {
        draw_text_centered(
            center_x,
            static_cast<unsigned short>(CELL_SIZE * ROWS * 0.5f),
            "No high scores yet!",
            window,
            sf::Color(128, 128, 128),
            0.8f
        );
    }
    
    // Draw instructions
    draw_text_centered(
        center_x,
        static_cast<unsigned short>(CELL_SIZE * ROWS * 0.9f),
        "Press ESC to return to menu",
        window,
        sf::Color::Magenta,
        0.6f
    );
    
    window.display();
}
