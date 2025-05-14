#pragma once
#include <SFML/Graphics.hpp>

namespace FRE {

	class Canvas
	{
	public:
		Canvas();
		Canvas(unsigned int width, unsigned height);
		void DrawLine(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color);
		void Clear(const sf::Color& color);
		void Display();
		sf::Vector2u GetSize();
		sf::Sprite& getSprite();
		sf::RenderTexture& getRenderTexture();
		void Update(const sf::RenderWindow& window, const sf::View& view);

	private:
		sf::RenderTexture m_RenderTexture;
		sf::Vector2u m_Size;
		std::optional<sf::Sprite> m_Sprite;
	};

}

