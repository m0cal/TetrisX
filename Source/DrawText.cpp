#include <SFML/Graphics.hpp>

#include "Headers/DrawText.hpp"

void draw_text(unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window)
{
    draw_text(i_x, i_y, i_text, i_window, sf::Color::White);
}

void draw_text(unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window, sf::Color color)
{
	//We're gonna align the text ot the left top
	short character_x = i_x;
	short character_y = i_y;

	unsigned char character_width;
	sf::Texture font_texture;
	if (!font_texture.loadFromFile("Resources/Images/Font.png"))
	{
		// Handle error - could not load font
		return;
	}

	//We're gonna calculate the width of the character based on the font image size
	//96 because there are 96 character in the image
	character_width = font_texture.getSize().x / 96;

	sf::Sprite character_sprite(font_texture);
	character_sprite.setColor(color);

	for (const char a : i_text)
	{
		if ('\n' == a)
		{
			//After every newline we put increase the y-coordinate and reset the x-coordinate
			character_x = i_x;
			character_y += static_cast<int>(font_texture.getSize().y);

			continue;
		}

		//Change the position of the next character
		character_sprite.setPosition(sf::Vector2f(character_x, character_y));
		//Pick the character from the font image
		character_sprite.setTextureRect(sf::IntRect({character_width * (a - 32), 0}, {static_cast<int>(character_width), static_cast<int>(font_texture.getSize().y)}));

		//Increase the x-coordinate
		character_x += character_width;

		//Draw the character
		i_window.draw(character_sprite);
	}
}

void draw_text_centered(unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window, sf::Color color, float scale)
{
	// Calculate text width to center it
	unsigned short text_width = get_text_width(i_text, scale);
	unsigned short centered_x = i_x - text_width / 2;
	
	//We're gonna align the text to the center
	short character_x = centered_x;
	short character_y = i_y;

	unsigned char character_width;
	sf::Texture font_texture;
	if (!font_texture.loadFromFile("Resources/Images/Font.png"))
	{
		// Handle error - could not load font
		return;
	}

	//We're gonna calculate the width of the character based on the font image size
	//96 because there are 96 character in the image
	character_width = font_texture.getSize().x / 96;
	sf::Sprite character_sprite(font_texture);
	character_sprite.setColor(color);
	character_sprite.setScale(sf::Vector2f(scale, scale));

	for (const char a : i_text)
	{
		if ('\n' == a)
		{
			//After every newline we put increase the y-coordinate and reset the x-coordinate
			character_x = centered_x;
			character_y += static_cast<int>(font_texture.getSize().y * scale);

			continue;
		}

		//Change the position of the next character
		character_sprite.setPosition(sf::Vector2f(character_x, character_y));
		//Pick the character from the font image
		character_sprite.setTextureRect(sf::IntRect({character_width * (a - 32), 0}, {static_cast<int>(character_width), static_cast<int>(font_texture.getSize().y)}));

		//Increase the x-coordinate
		character_x += static_cast<int>(character_width * scale);

		//Draw the character
		i_window.draw(character_sprite);
	}
}

unsigned short get_text_width(const std::string& i_text, float scale)
{
	sf::Texture font_texture;
	if (!font_texture.loadFromFile("Resources/Images/Font.png"))
	{
		return 0;
	}

	unsigned char character_width = font_texture.getSize().x / 96;
	unsigned short width = 0;
	
	for (const char a : i_text)
	{
		if (a == '\n')
		{
			break; // Only calculate width of first line
		}
		width += static_cast<unsigned short>(character_width * scale);
	}
	
	return width;
}