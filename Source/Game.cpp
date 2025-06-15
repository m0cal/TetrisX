#include "Headers/Game.hpp"

Game::Game() : 
    lag(0),
    tetromino(state.generate_next_shape(), state.get_matrix()),
    window(sf::VideoMode({2 * CELL_SIZE * COLUMNS * SCREEN_RESIZE, CELL_SIZE * ROWS * SCREEN_RESIZE}), "Tetris", sf::Style::Close),
    renderer(window)
{
    setup_window();
    state.set_next_shape(state.generate_next_shape());
    previous_time = std::chrono::steady_clock::now();
}

void Game::setup_window()
{
    // Set window position to center of screen
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int windowWidth = 2 * CELL_SIZE * COLUMNS * SCREEN_RESIZE;
    unsigned int windowHeight = CELL_SIZE * ROWS * SCREEN_RESIZE;
    window.setPosition(sf::Vector2i((desktop.size.x - windowWidth) / 2, (desktop.size.y - windowHeight) / 2));
    
    // Set view
    window.setView(sf::View(sf::FloatRect({0, 0}, {2 * CELL_SIZE * COLUMNS, CELL_SIZE * ROWS})));
}

void Game::run()
{
    while (window.isOpen())
    {
        unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - previous_time).count();
        
        lag += delta_time;
        previous_time += std::chrono::microseconds(delta_time);
        
        while (FRAME_DURATION <= lag)
        {
            lag -= FRAME_DURATION;
            update();
        }
        
        renderer.render_frame(state, tetromino, lag);
    }
}

void Game::update()
{
    InputHandler::handle_events(window, state);
    
    if (state.get_clear_effect_timer() == 0)
    {
        InputHandler::handle_gameplay_input(state, tetromino);
        
        if (!state.is_game_over())
        {
            handle_tetromino_falling();
        }
    }
    else
    {
        LineClearing::process_line_clearing_effect(state);
        
        if (state.get_clear_effect_timer() == 0)
        {
            bool game_over = !tetromino.reset(state.get_next_shape(), state.get_matrix());
            state.set_game_over(game_over);
            state.set_next_shape(state.generate_next_shape());
        }
    }
}

void Game::handle_tetromino_falling()
{
    if (state.get_fall_timer() == state.get_current_fall_speed())
    {
        if (!tetromino.move_down(state.get_matrix()))
        {
            handle_tetromino_placement();
        }
        state.set_fall_timer(0);
    }
    else
    {
        state.increment_fall_timer();
    }
}

void Game::handle_tetromino_placement()
{
    tetromino.update_matrix(state.get_matrix());
    LineClearing::check_and_mark_lines_for_clearing(state);
    
    if (state.get_clear_effect_timer() == 0)
    {
        bool game_over = !tetromino.reset(state.get_next_shape(), state.get_matrix());
        state.set_game_over(game_over);
        state.set_next_shape(state.generate_next_shape());
    }
}
