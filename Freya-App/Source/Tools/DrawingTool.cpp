#include "DrawingTool.h"
#include <iostream>
#include <cmath>

using namespace FRE;

DrawingTool::DrawingTool(Canvas& canvas)
    : Tool(canvas)
{
    // The constructor of the Tool base class already stores the Canvas reference
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
				// Save canvas state before begining a new drawing operation
                m_Canvas.BeginDrawOperation();

                m_IsDrawing = true;
                m_LastPosition = mouseWorldPos - m_Canvas.GetSprite().getPosition();
                m_AccumulatedDistance = 0.f; // Reset accumulated distance when starting new drawing

                // Starting point is drawn as a point
                if (m_Thickness <= 1.f)
                {
                    sf::Vertex pixel(m_LastPosition, m_Color);
                    m_Canvas.GetRenderTexture().draw(&pixel, 1, sf::PrimitiveType::Points);
                }
                else
                {
                    sf::CircleShape dot(m_Thickness);
                    dot.setFillColor(m_Color);
                    dot.setPosition(m_LastPosition - sf::Vector2f(m_Thickness, m_Thickness));
                    m_Canvas.GetRenderTexture().draw(dot);
                }

            }
        }
    }

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
    {
		m_Canvas.EndDrawOperation(); // Save the current state of the canvas
        m_IsDrawing = false;
    }
}

void DrawingTool::Update(const sf::RenderWindow& window, const sf::View& view, UI& ui)
{
    if (!m_IsDrawing) return;

    SetColor(ui.GetColor());
    SetThickness(ui.GetBrushSize());
    SetSpacing(ui.GetSpacing());

    sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.GetSprite().getPosition();

    if (m_LastPosition == newPosition)
        return;

    float distanceMoved = std::hypot(newPosition.x - m_LastPosition.x, newPosition.y - m_LastPosition.y);

    float spacingDistance = (m_Spacing > 0) ? m_Thickness * (1.0f + m_Spacing * 0.05f) : 0.f;
    m_AccumulatedDistance += distanceMoved;

    if (m_AccumulatedDistance >= spacingDistance || m_Spacing == 0)
    {
        if (m_Thickness <= 1.f)
        {
            if (m_Spacing > 0)
            {
                // 1 pixel point
                sf::Vertex pixel(m_LastPosition, m_Color);
                m_Canvas.GetRenderTexture().draw(&pixel, 1, sf::PrimitiveType::Points);
            }
            else
            {
                sf::Vertex line[] = {
                    sf::Vertex(m_LastPosition, m_Color),
                    sf::Vertex(newPosition, m_Color)
                };
                m_Canvas.GetRenderTexture().draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
        else
        {
            if (m_Spacing > 0)
            {

                sf::CircleShape dot(m_Thickness);
                dot.setFillColor(m_Color);
                dot.setPosition(m_LastPosition - sf::Vector2f(m_Thickness, m_Thickness));
                m_Canvas.GetRenderTexture().draw(dot);
            }
            else
            {
                for (int dx = -static_cast<int>(m_Thickness); dx <= static_cast<int>(m_Thickness); ++dx)
                {
                    for (int dy = -static_cast<int>(m_Thickness); dy <= static_cast<int>(m_Thickness); ++dy)
                    {
                        if (dx * dx + dy * dy <= m_Thickness * m_Thickness)
                        {
                            sf::Vector2f offset(static_cast<float>(dx), static_cast<float>(dy));

                            sf::Vertex line[] = {
                                sf::Vertex(m_LastPosition + offset, m_Color),
                                sf::Vertex(newPosition + offset, m_Color)
                            };

                            m_Canvas.GetRenderTexture().draw(line, 2, sf::PrimitiveType::Lines);
                        }
                    }
                }
            }
        }

        if (m_Spacing > 0)
        {
            while (m_AccumulatedDistance >= spacingDistance)
            {
                m_AccumulatedDistance -= spacingDistance;
            }
        }
        else
        {
            m_AccumulatedDistance = 0.f;
        }
    }

    m_LastPosition = newPosition;
}
