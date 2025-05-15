#include "DrawingTool.h"
#include <iostream>

using namespace FRE;

DrawingTool::DrawingTool(Canvas& canvas)
	: m_Canvas(canvas)
{
	
}

void DrawingTool::HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view)
{
    // Convert mouse coordinates to world coordinates
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
    sf::FloatRect canvasBounds = m_Canvas.GetSprite().getGlobalBounds();

    // Allow drawing only if it starts within the canvas area
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            if (canvasBounds.contains(mouseWorldPos))
            {
                m_IsDrawing = true;
                m_LastPosition = mouseWorldPos - m_Canvas.GetSprite().getPosition();
                m_AccumulatedDistance = 0.f; // Reset accumulated distance when starting new drawing

                // Starting point is drawn as a circle
                sf::CircleShape dot(m_Thickness);
                dot.setFillColor(m_Color);
                dot.setPosition(m_LastPosition - sf::Vector2f(m_Thickness, m_Thickness));
                m_Canvas.GetRenderTexture().draw(dot);
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

    sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.GetSprite().getPosition();

    if (m_LastPosition == newPosition)
        return;

    // Calculate the distance to be used for spacing
    float distanceMoved = std::sqrt(std::pow(newPosition.x - m_LastPosition.x, 2) +
        std::pow(newPosition.y - m_LastPosition.y, 2));

    // Calculate spacing distance for integer spacing value (0-500)
    // Continuous drawing when Spacing is 0, maximum spacing when 500
    float spacingDistance = 0.f;
    if (m_Spacing > 0) {
        // Calculate distance based on brush thickness and spacing value
        // As the spacing value increases, the dot spacing also increases
        spacingDistance = m_Thickness * (1.0f + m_Spacing * 0.05f);
    }

    m_AccumulatedDistance += distanceMoved;

    // Draw if accumulated distance reaches spacing distance or spacing is 0
    if (m_AccumulatedDistance >= spacingDistance || m_Spacing == 0)
    {
        // If spacing is active and enough distance has accumulated, draw as dots
        if (m_Spacing > 0)
        {
            // Draw dots instead of lines
            sf::CircleShape dot(m_Thickness);
            dot.setFillColor(m_Color);
            dot.setPosition(newPosition - sf::Vector2f(m_Thickness, m_Thickness));
            m_Canvas.GetRenderTexture().draw(dot);

            // Decrease accumulated distance as you move by multiples of the spacing distance
            while (m_AccumulatedDistance >= spacingDistance)
            {
                m_AccumulatedDistance -= spacingDistance;
            }
        }
        else // If Spacing is 0 draw normal line
        {
            // Draw lines around the brush thickness
            for (int dx = -static_cast<int>(m_Thickness); dx <= static_cast<int>(m_Thickness); ++dx) {
                for (int dy = -static_cast<int>(m_Thickness); dy <= static_cast<int>(m_Thickness); ++dy) {
                    if (dx * dx + dy * dy <= m_Thickness * m_Thickness) { // Circular brush effect
                        sf::Vector2f offset(static_cast<float>(dx), static_cast<float>(dy));

                        sf::Vertex line[] = {
                            sf::Vertex(m_LastPosition + offset, m_Color),
                            sf::Vertex(newPosition + offset, m_Color)
                        };

                        m_Canvas.GetRenderTexture().draw(line, 2, sf::PrimitiveType::Lines);
                    }
                }
            }

            // Reset accumulated distance for continuous drawing mode
            m_AccumulatedDistance = 0.f;
        }
    }

    m_LastPosition = newPosition;
}

void DrawingTool::SetColor(sf::Color color)
{
	m_Color = color;
}

void DrawingTool::SetThickness(float thickness)
{
    m_Thickness = std::clamp(thickness, 1.f, m_MaxThickness);
}

float DrawingTool::GetThickness()
{
	return m_Thickness;
}

void DrawingTool::SetSpacing(int spacing) {
    m_Spacing = std::max(0, spacing);
}