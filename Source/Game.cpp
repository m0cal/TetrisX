#include "Headers/Game.hpp"

Game::Game() : 
    lag(0),
    failure_sound_played(false),
    score_saved(false),
    previous_game_mode(GameMode::MENU),
    last_lines_cleared_count(0),
    tetromino(state.generate_next_shape(), state.get_matrix()),
    window(sf::VideoMode({2 * CELL_SIZE * COLUMNS * SCREEN_RESIZE, CELL_SIZE * ROWS * SCREEN_RESIZE}), "Tetris", sf::Style::Close),
    renderer(window),
    leaderboard_manager()
{
    setup_window();
    state.set_next_shape(state.generate_next_shape());
    previous_time = std::chrono::steady_clock::now();
    
    // Start background music
    audio_manager.play_background_music();
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
        
        renderer.render_frame(state, tetromino, lag, &leaderboard_manager);
    }
}

void Game::update()
{
    // Check if game has been restarted (from GAME_OVER to PLAYING or MENU to PLAYING)
    GameMode current_mode = state.get_current_mode();
    if ((previous_game_mode == GameMode::GAME_OVER || previous_game_mode == GameMode::MENU) 
        && current_mode == GameMode::PLAYING)
    {
        failure_sound_played = false;
        score_saved = false; // Reset score saved flag when starting new game
    }
    previous_game_mode = current_mode;
    
    InputHandler::handle_events(window, state, audio_manager);
    
    if (state.get_current_mode() == GameMode::MENU)
    {
        InputHandler::handle_menu_input(state);
        InputHandler::handle_menu_mouse_input(window, state);
        
        // Check if user selected exit (keyboard or mouse)
        if (state.get_selected_menu_option() == 2)
        {
            static bool enter_pressed = false;
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter) && !enter_pressed)
            {
                enter_pressed = true;
                window.close();
            }
            else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter))
            {
                enter_pressed = false;
            }
        }
        
        // Check if exit was requested via mouse click
        if (state.is_exit_requested())
        {
            window.close();
        }
    }
    else if (state.get_current_mode() == GameMode::LEADERBOARD)
    {
        // Handle leaderboard input - ESC to return to menu
        static bool esc_pressed = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape) && !esc_pressed)
        {
            esc_pressed = true;
            state.set_current_mode(GameMode::MENU);
            state.set_selected_menu_option(0);
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape))
        {
            esc_pressed = false;
        }
    }
    else if (state.get_current_mode() == GameMode::PLAYING || state.get_current_mode() == GameMode::GAME_OVER)
    {
        if (state.get_clear_effect_timer() == 0)
        {
            InputHandler::handle_gameplay_input(state, tetromino);
            
            if (!state.is_game_over())
            {
                handle_tetromino_falling();
            }
            else
            {
                handle_game_over();
            }
        }
        else
        {
            LineClearing::process_line_clearing_effect(state);
            
            if (state.get_clear_effect_timer() == 0)
            {
                bool game_over = !tetromino.reset(state.get_next_shape(), state.get_matrix());
                if (game_over && !failure_sound_played)
                {
                    audio_manager.play_sound(SoundType::FAILURE);
                    failure_sound_played = true;
                }
                state.set_game_over(game_over);
                if (game_over)
                {
                    state.set_current_mode(GameMode::GAME_OVER);
                }
                state.set_next_shape(state.generate_next_shape());
            }
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
    audio_manager.play_sound(SoundType::DROP);
    
    // Add tetromino placement score
    state.get_score_system().add_tetromino_placed_score();
    
    // Store lines cleared before clearing
    unsigned int lines_before = state.get_lines_cleared();
    
    LineClearing::check_and_mark_lines_for_clearing(state, audio_manager);
    
    // Calculate lines cleared this placement for scoring
    unsigned int lines_cleared_now = state.get_lines_cleared() - lines_before;
    if (lines_cleared_now > 0)
    {
        state.get_score_system().add_line_clear_score(lines_cleared_now, state.get_lines_cleared());
    }
    else
    {
        state.get_score_system().reset_combo();
    }
    
    if (state.get_clear_effect_timer() == 0)
    {
        bool game_over = !tetromino.reset(state.get_next_shape(), state.get_matrix());
        if (game_over && !failure_sound_played)
        {
            audio_manager.play_sound(SoundType::FAILURE);
            failure_sound_played = true;
        }
        state.set_game_over(game_over);
        if (game_over)
        {
            state.set_current_mode(GameMode::GAME_OVER);
        }
        state.set_next_shape(state.generate_next_shape());
    }
}

void Game::handle_game_over()
{
    // Only save score once per game over
    if (!score_saved)
    {
        // Check if this is a high score
        unsigned long final_score = state.get_score_system().get_current_score();
        if (leaderboard_manager.is_high_score(final_score))
        {
            // For now, use "Player" as default name
            // In a full implementation, you'd want to get the player's name via input
            ScoreEntry entry = state.get_score_system().create_score_entry("Player", state.get_lines_cleared());
            leaderboard_manager.add_score(entry);
        }
        score_saved = true; // Mark score as saved
    }
}
