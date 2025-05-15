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
		void SetThickness(float thickness);
		float GetThickness();

	private:
		Canvas& m_Canvas;
		bool m_IsDrawing = false;
		float m_Thickness = 1.f;
		float m_MaxThickness = 100.f;
		float m_Hardness = 1.f;
		float m_Spacing = 1.f;
		float m_Opacity = 1.f;
		
		float m_DistanceAccumulator = 0.f;

		sf::Vector2f m_LastPosition;
		sf::Color m_Color;

		void DrawBrush(sf::Vector2f position);
		float Distance(const sf::Vector2f& a, const sf::Vector2f& b);
	};

}

