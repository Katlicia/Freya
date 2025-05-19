#include "EraserTool.h"
#include <cmath>

using namespace FRE;

EraserTool::EraserTool(Canvas& canvas)
    : Tool(canvas)
{
    // The constructor of the Tool base class already stores the Canvas reference
}

void EraserTool::HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view)
{
    // Convert mouse coordinates to world coordinates
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
    sf::FloatRect canvasBounds = m_Canvas.GetSprite().getGlobalBounds();

    // Allow erasing only if it starts within the canvas area
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            if (canvasBounds.contains(mouseWorldPos))
            {
                // Save canvas state before beginning a new erasing operation
                m_Canvas.BeginDrawOperation();

                m_IsErasing = true;
                m_LastPosition = mouseWorldPos - m_Canvas.GetSprite().getPosition();
                m_AccumulatedDistance = 0.f; // Reset accumulated distance when starting new erasing

                sf::RenderStates states;
                states.blendMode = sf::BlendNone; // Deletes existing pixels

                if (m_Thickness <= 1.f)
                {
                    // Erase a single pixel
                    sf::Vertex pixel(m_LastPosition, sf::Color(0, 0, 0, 0));
                    m_Canvas.GetRenderTexture().draw(&pixel, 1, sf::PrimitiveType::Points, states);
                }
                else
                {
                    // Erase using a circular area
                    sf::CircleShape eraser(m_Thickness);
                    eraser.setFillColor(sf::Color(0, 0, 0, 0)); // Fully transparent color
                    eraser.setPosition(m_LastPosition - sf::Vector2f(m_Thickness, m_Thickness));
                    m_Canvas.GetRenderTexture().draw(eraser, states);
                }
            }
        }
    }

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
    {
        m_Canvas.EndDrawOperation(); // Save the current state of the canvas
        m_IsErasing = false;
    }
}


void EraserTool::Update(const sf::RenderWindow& window, const sf::View& view, UI& ui)
{
    if (!m_IsErasing) return;

    SetThickness(ui.GetBrushSize()); // Update eraser tool thickness
    SetSpacing(ui.GetSpacing());     // Update eraser tool spacing

    sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.GetSprite().getPosition();

    if (m_LastPosition == newPosition)
        return;

    float distanceMoved = std::hypot(newPosition.x - m_LastPosition.x, newPosition.y - m_LastPosition.y);

    float spacingDistance = (m_Spacing > 0) ? m_Thickness * (1.0f + m_Spacing * 0.05f) : 0.f;
    m_AccumulatedDistance += distanceMoved;

    if (m_AccumulatedDistance >= spacingDistance || m_Spacing == 0)
    {
        sf::RenderStates states;
        states.blendMode = sf::BlendNone; // Fully erase pixels

        if (m_Thickness <= 1.f)
        {
            if (m_Spacing > 0)
            {
                // Erase single pixel
                sf::Vertex pixel(newPosition, sf::Color(0, 0, 0, 0));
                m_Canvas.GetRenderTexture().draw(&pixel, 1, sf::PrimitiveType::Points, states);
            }
            else
            {
                // Erase a thin line between two points
                sf::Vertex line[] = {
                    sf::Vertex(m_LastPosition, sf::Color(0, 0, 0, 0)),
                    sf::Vertex(newPosition, sf::Color(0, 0, 0, 0))
                };
                m_Canvas.GetRenderTexture().draw(line, 2, sf::PrimitiveType::Lines, states);
            }
        }
        else
        {
            if (m_Spacing > 0)
            {
                // Erase as a circle
                sf::CircleShape eraser(m_Thickness);
                eraser.setFillColor(sf::Color(0, 0, 0, 0));
                eraser.setPosition(newPosition - sf::Vector2f(m_Thickness, m_Thickness));
                m_Canvas.GetRenderTexture().draw(eraser, states);
            }
            else
            {
                // Erase a thick path between last and current position
                for (int dx = -static_cast<int>(m_Thickness); dx <= static_cast<int>(m_Thickness); ++dx) {
                    for (int dy = -static_cast<int>(m_Thickness); dy <= static_cast<int>(m_Thickness); ++dy) {
                        if (dx * dx + dy * dy <= m_Thickness * m_Thickness) {
                            sf::Vector2f offset(static_cast<float>(dx), static_cast<float>(dy));

                            float length = std::hypot(newPosition.x - m_LastPosition.x, newPosition.y - m_LastPosition.y);
                            float angle = std::atan2(newPosition.y - m_LastPosition.y, newPosition.x - m_LastPosition.x) * 180.0f / 3.14159265f;

                            sf::RectangleShape rect(sf::Vector2f(length, 2.0f));
                            rect.setFillColor(sf::Color(0, 0, 0, 0));
                            rect.setOrigin({ 0, 1.0f });
                            rect.setPosition(m_LastPosition + offset);
                            rect.setRotation(sf::degrees(angle));

                            m_Canvas.GetRenderTexture().draw(rect, states);
                        }
                    }
                }

                // Erase at final position for smooth result
                sf::CircleShape eraser(m_Thickness);
                eraser.setFillColor(sf::Color(0, 0, 0, 0));
                eraser.setPosition(newPosition - sf::Vector2f(m_Thickness, m_Thickness));
                m_Canvas.GetRenderTexture().draw(eraser, states);
            }
        }

        // Reset or decrement accumulated distance
        if (m_Spacing > 0) {
            while (m_AccumulatedDistance >= spacingDistance)
                m_AccumulatedDistance -= spacingDistance;
        }
        else {
            m_AccumulatedDistance = 0.f;
        }
    }

    m_LastPosition = newPosition;
}
