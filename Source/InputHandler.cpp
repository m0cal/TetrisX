#include "Headers/InputHandler.hpp"
#include "Headers/AudioManager.hpp"
#include <iostream>

void InputHandler::handle_events(sf::RenderWindow& window, GameState& state, AudioManager& audio_manager)
{
    while (auto event = window.pollEvent())
    {
        if (const auto* closeEvent = event->getIf<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
        {
            switch (keyEvent->scancode)
            {
                case sf::Keyboard::Scancode::M:
                    audio_manager.toggle_mute();
                    break;
            }
        }
        else if (const auto* keyEvent = event->getIf<sf::Event::KeyReleased>())
        {
            switch (keyEvent->scancode)
            {
                case sf::Keyboard::Scancode::C:
                case sf::Keyboard::Scancode::Z:
                    state.set_rotate_pressed(false);
                    break;
                case sf::Keyboard::Scancode::Down:
                    state.set_soft_drop_timer(0);
                    break;
                case sf::Keyboard::Scancode::Left:
                case sf::Keyboard::Scancode::Right:
                    state.set_move_timer(0);
                    break;
                case sf::Keyboard::Scancode::Space:
                    state.set_hard_drop_pressed(false);
                    break;
            }
        }
    }
}

void InputHandler::handle_gameplay_input(GameState& state, Tetromino& tetromino)
{
    if (state.get_clear_effect_timer() == 0)
    {
        if (!state.is_game_over())
        {
            handle_rotation_input(state, tetromino);
            handle_movement_input(state, tetromino);
            handle_drop_input(state, tetromino);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter))
        {
            state.set_current_mode(GameMode::MENU);
            state.set_selected_menu_option(0);
            state.set_exit_requested(false); // Reset exit flag when returning to menu
        }
    }
}

void InputHandler::handle_rotation_input(GameState& state, Tetromino& tetromino)
{
    if (!state.is_rotate_pressed())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::C))
        {
            state.set_rotate_pressed(true);
            tetromino.rotate(1, state.get_matrix());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z))
        {
            state.set_rotate_pressed(true);
            tetromino.rotate(0, state.get_matrix());
        }
    }
}

void InputHandler::handle_movement_input(GameState& state, Tetromino& tetromino)
{
    if (state.get_move_timer() == 0)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
        {
            state.set_move_timer(1);
            tetromino.move_left(state.get_matrix());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
        {
            state.set_move_timer(1);
            tetromino.move_right(state.get_matrix());
        }
    }
    else
    {
        state.update_move_timer();
    }
}

void InputHandler::handle_drop_input(GameState& state, Tetromino& tetromino)
{
    // Hard drop
    if (!state.is_hard_drop_pressed())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space))
        {
            state.set_hard_drop_pressed(true);
            state.set_fall_timer(state.get_current_fall_speed());
            unsigned char drop_distance = tetromino.hard_drop(state.get_matrix());
            
            // Add hard drop score
            state.get_score_system().add_drop_score(true, drop_distance);
        }
    }
    
    // Soft drop
    if (state.get_soft_drop_timer() == 0)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down))
        {
            if (tetromino.move_down(state.get_matrix()))
            {
                state.set_fall_timer(0);
                state.set_soft_drop_timer(1);
                
                // Add soft drop score
                state.get_score_system().add_drop_score(false, 1);
            }
        }
    }
    else
    {
        state.update_soft_drop_timer();
    }
}

void InputHandler::handle_menu_input(GameState& state)
{
    static bool up_pressed = false;
    static bool down_pressed = false;
    static bool enter_pressed = false;
    
    // Handle navigation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up) && !up_pressed)
    {
        up_pressed = true;
        state.set_selected_menu_option((state.get_selected_menu_option() - 1 + 3) % 3);
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up))
    {
        up_pressed = false;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down) && !down_pressed)
    {
        down_pressed = true;
        state.set_selected_menu_option((state.get_selected_menu_option() + 1) % 3);
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down))
    {
        down_pressed = false;
    }
    
    // Handle selection
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter) && !enter_pressed)
    {
        enter_pressed = true;
        if (state.get_selected_menu_option() == 0)
        {
            // Start game
            state.reset_game();
        }
        else if (state.get_selected_menu_option() == 1)
        {
            // Show leaderboard
            state.set_current_mode(GameMode::LEADERBOARD);
        }
        // Option 2 (Exit) is handled in the main game loop
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter))
    {
        enter_pressed = false;
    }
}

void InputHandler::handle_menu_mouse_input(sf::RenderWindow& window, GameState& state)
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos);
    
    // Convert to game coordinates
    float game_x = world_pos.x;
    float game_y = world_pos.y;
    
    // Define menu button areas - matching the exact text positions from Renderer.cpp
    // Text is centered at x = CELL_SIZE * COLUMNS = 80
    // START button is at y = CELL_SIZE * ROWS * 0.4f = 64
    // LEADERBOARD button is at y = CELL_SIZE * ROWS * 0.5f = 80
    // EXIT button is at y = CELL_SIZE * ROWS * 0.6f = 96
    float start_button_y = CELL_SIZE * ROWS * 0.4f - CELL_SIZE * 0.5f;  // Center around text
    float leaderboard_button_y = CELL_SIZE * ROWS * 0.5f - CELL_SIZE * 0.5f;  // Center around text
    float exit_button_y = CELL_SIZE * ROWS * 0.6f - CELL_SIZE * 0.5f;   // Center around text
    float button_height = CELL_SIZE;   // One cell height for better clickability
    
    // Center the clickable area around the text
    float center_x = CELL_SIZE * COLUMNS; // 80
    float button_half_width = CELL_SIZE * COLUMNS * 0.4f; // 32
    float button_left = center_x - button_half_width;     // 48
    float button_right = center_x + button_half_width;    // 112
    
    static bool mouse_clicked = false;
    bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    
    // Check if mouse is over START button
    if (game_x >= button_left && game_x <= button_right &&
        game_y >= start_button_y && game_y <= start_button_y + button_height)
    {
        state.set_selected_menu_option(0);
        
        if (mouse_pressed && !mouse_clicked)
        {
            mouse_clicked = true;
            state.reset_game();
        }
    }
    // Check if mouse is over LEADERBOARD button
    else if (game_x >= button_left && game_x <= button_right &&
             game_y >= leaderboard_button_y && game_y <= leaderboard_button_y + button_height)
    {
        state.set_selected_menu_option(1);
        
        if (mouse_pressed && !mouse_clicked)
        {
            mouse_clicked = true;
            state.set_current_mode(GameMode::LEADERBOARD);
        }
    }
    // Check if mouse is over EXIT button
    else if (game_x >= button_left && game_x <= button_right &&
             game_y >= exit_button_y && game_y <= exit_button_y + button_height)
    {
        state.set_selected_menu_option(2);
        
        if (mouse_pressed && !mouse_clicked)
        {
            mouse_clicked = true;
            state.set_exit_requested(true);
        }
    }
    
    // Reset mouse click state when button is released
    if (!mouse_pressed)
    {
        mouse_clicked = false;
    }
}
