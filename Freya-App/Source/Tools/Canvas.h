#pragma once
#include <SFML/Graphics.hpp>

namespace FRE {

	class Canvas
	{
	public:
		Canvas();
		Canvas(unsigned int width, unsigned height, bool transparant);
		void Update(const sf::RenderWindow& window, const sf::View& view);
		void Display();
		void DrawLine(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color);
		void Clear(const sf::Color& color);
		sf::Vector2u GetSize();
		void SetSize(unsigned int width, unsigned int height);
		sf::Sprite& GetSprite();
		sf::RenderTexture& GetRenderTexture();

		bool ExportToPNG(const std::string& filename);
		bool ExportToJPG(const std::string& filename);
		bool ExportToBMP(const std::string& filename);

		void CreateTransparencyPattern();
		void DrawTransparencyPattern(sf::RenderTarget& target);


	private:
		sf::RenderTexture m_RenderTexture;
		sf::Vector2u m_Size;
		std::optional<sf::Sprite> m_Sprite;

		sf::Texture m_TransparencyPatternTexture;
		std::optional<sf::Sprite> m_TransparencyPatternSprite;
		bool m_HasTransparencyPattern = false;
		bool m_Transparant = false;
	};

}

