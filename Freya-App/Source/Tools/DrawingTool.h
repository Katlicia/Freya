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

		float GetThickness();

		void SetColor(sf::Color color);
		void SetThickness(float thickness);
		void SetSpacing(float spacing);

	private:
		Canvas& m_Canvas;
		bool m_IsDrawing = false;
		float m_Thickness = 1.f;
		float m_MaxThickness = 100.f;
		float m_Spacing = 1.f;

		sf::Vector2f m_LastPosition;
		sf::Color m_Color;

	};

}

