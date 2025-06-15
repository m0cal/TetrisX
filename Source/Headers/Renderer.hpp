#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Global.hpp"
#include "GameState.hpp"
#include "Tetromino.hpp"

class Renderer
{
private:
    std::vector<sf::Color> cell_colors;
    sf::RectangleShape cell;
    sf::RenderWindow& window;

public:
    explicit Renderer(sf::RenderWindow& window);
    
    void render_frame(const GameState& state, const Tetromino& tetromino, unsigned lag);
    void render_menu(const GameState& state);
    
private:
    void render_matrix(const GameState& state);
    void render_tetromino(const Tetromino& tetromino, const GameState& state);
    void render_ghost_tetromino(const Tetromino& tetromino, const GameState& state);
    void render_line_clear_effect(const GameState& state);
    void render_next_tetromino_preview(const GameState& state);
    void render_ui(const GameState& state);
    
    unsigned char calculate_clear_cell_size(unsigned char clear_effect_timer) const;
};
