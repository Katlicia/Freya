#include "DrawingTool.h"

using namespace FRE;

DrawingTool::DrawingTool(Canvas& canvas)
	: m_Canvas(canvas)
{
	
}

void DrawingTool::HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view)
{
	sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
	sf::FloatRect canvasBounds = m_Canvas.getSprite().getGlobalBounds();
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
			if (canvasBounds.contains(mouseWorldPos)) // If mouse is in canvas
			{
				m_IsDrawing = true;
				m_LastPosition = mouseWorldPos - m_Canvas.getSprite().getPosition();
			}
        }
    }

	if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
	{
		m_IsDrawing = false;
	}

}

void DrawingTool::Update(const sf::RenderWindow& window, const sf::View& view)
{
	if (m_IsDrawing)
	{
		sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.getSprite().getPosition();
		// If position changed add new line
		if (m_LastPosition != newPosition)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(m_LastPosition, sf::Color::Black),
				sf::Vertex(newPosition, sf::Color::Black)
			};
			m_Canvas.getRenderTexture().draw(line, 2, sf::PrimitiveType::Lines);
			m_LastPosition = newPosition;
		}
	}
}