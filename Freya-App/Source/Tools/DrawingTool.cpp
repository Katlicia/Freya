#include "DrawingTool.h"
#include <iostream>

using namespace FRE;

DrawingTool::DrawingTool(Canvas& canvas)
	: m_Canvas(canvas)
{
	
}

//void DrawingTool::HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view)
//{
//	sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
//	sf::FloatRect canvasBounds = m_Canvas.getSprite().getGlobalBounds();
//
//    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
//    {
//        if (mouseEvent->button == sf::Mouse::Button::Left)
//        {
//			if (canvasBounds.contains(mouseWorldPos)) // If mouse is in canvas
//			{
//				m_IsDrawing = true;
//				m_LastPosition = mouseWorldPos - m_Canvas.getSprite().getPosition();
//				// Draw a point when clicking
//				sf::Vertex point[] =
//				{
//					sf::Vertex(m_LastPosition, m_Color),
//					sf::Vertex(m_LastPosition, m_Color)
//				};
//				m_Canvas.getRenderTexture().draw(point, 2, sf::PrimitiveType::Points);
//			}
//        }
//    }
//
//	if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
//	{
//		m_IsDrawing = false;
//	}
//
//}

void DrawingTool::HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view)
{
    // Mouse koordinatlarýný dünya (world) koordinatýna çevir
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
    sf::FloatRect canvasBounds = m_Canvas.getSprite().getGlobalBounds();

    // Sadece canvas alaný içinde baþlarsa çizime izin ver
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            if (canvasBounds.contains(mouseWorldPos))
            {
                m_IsDrawing = true;
                m_LastPosition = mouseWorldPos - m_Canvas.getSprite().getPosition();

                // Baþlangýç noktasý daire olarak çizilir
                sf::CircleShape dot(m_Thickness);
                dot.setFillColor(m_Color);
                dot.setPosition(m_LastPosition - sf::Vector2f(m_Thickness, m_Thickness));
                m_Canvas.getRenderTexture().draw(dot);
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
    if (!m_IsDrawing) return;

    sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.getSprite().getPosition();

    if (m_LastPosition == newPosition)
        return;

    // Brush kalýnlýðý kadar çevresine çizgiler çiz
    for (int dx = -static_cast<int>(m_Thickness); dx <= static_cast<int>(m_Thickness); ++dx) {
        for (int dy = -static_cast<int>(m_Thickness); dy <= static_cast<int>(m_Thickness); ++dy) {
            if (dx * dx + dy * dy <= m_Thickness * m_Thickness) { // dairesel fýrça etkisi
                sf::Vector2f offset(static_cast<float>(dx), static_cast<float>(dy));

                sf::Vertex line[] = {
                    sf::Vertex(m_LastPosition + offset, m_Color),
                    sf::Vertex(newPosition + offset, m_Color)
                };

                m_Canvas.getRenderTexture().draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
    }

    m_LastPosition = newPosition;
}



//void DrawingTool::Update(const sf::RenderWindow& window, const sf::View& view)
//{
//	if (m_IsDrawing)
//	{
//		sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.getSprite().getPosition();
//		// If position changed add new line
//		if (m_LastPosition != newPosition)
//		{
//			sf::Vertex line[] =
//			{
//				sf::Vertex(m_LastPosition, m_Color),
//				sf::Vertex(newPosition, m_Color)
//			};
//			m_Canvas.getRenderTexture().draw(line, 2, sf::PrimitiveType::Lines);
//			m_LastPosition = newPosition;
//		}
//
//	}
//}

void DrawingTool::SetColor(sf::Color color)
{
	//std::cout << color[0] << " " << color[1] << " " << color[2] << std::endl;
	m_Color = color;
}

//void DrawingTool::setThickness(float thickness)
//{
//    if (thickness <= m_MaxThickness) 
//    {
//	    m_Thickness = std::max(1.f, thickness);
//    }
//}

void DrawingTool::SetThickness(float thickness)
{
    m_Thickness = std::clamp(thickness, 1.f, m_MaxThickness);
}

float DrawingTool::GetThickness()
{
	return m_Thickness;
}