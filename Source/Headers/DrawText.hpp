#pragma once
#include <SFML/Graphics.hpp>

void draw_text(unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window);
void draw_text(unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window, sf::Color color);
void draw_text_centered(unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window, sf::Color color, float scale = 1.0f);
unsigned short get_text_width(const std::string& i_text, float scale = 1.0f);