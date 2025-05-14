#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Canvas.h"

namespace FRE {

	class DrawingTool
	{
	public:
		DrawingTool(Canvas& canvas);

		void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view);
		void Update(const sf::RenderWindow& window, const sf::View& view);

		void SetColor(sf::Color color);

	private:
		Canvas& m_Canvas;
		bool m_IsDrawing = false;
		sf::Vector2f m_LastPosition;
		sf::Color m_Color;
	};

}

