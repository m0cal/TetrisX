#include "Headers/InputHandler.hpp"

void InputHandler::handle_events(sf::RenderWindow& window, GameState& state)
{
    while (auto event = window.pollEvent())
    {
        if (const auto* closeEvent = event->getIf<sf::Event::Closed>())
        {
            window.close();
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
            state.reset_game();
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
            tetromino.hard_drop(state.get_matrix());
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
            }
        }
    }
    else
    {
        state.update_soft_drop_timer();
    }
}
