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
                // Save canvas state before begining a new drawing operation
                m_Canvas.BeginDrawOperation();
                m_IsErasing = true;
                m_LastPosition = mouseWorldPos - m_Canvas.GetSprite().getPosition();
                m_AccumulatedDistance = 0.f; // Reset accumulated distance when starting new erasing

                // Starting point is erased as a circle
                sf::CircleShape eraser(m_Thickness);
                eraser.setFillColor(sf::Color(0, 0, 0, 0)); // Fully transparent color
                eraser.setPosition(m_LastPosition - sf::Vector2f(m_Thickness, m_Thickness));

                // Eraser blend mode for transparency
                sf::RenderStates states;
                states.blendMode = sf::BlendNone; // Deletes existing pixels
                m_Canvas.GetRenderTexture().draw(eraser, states);
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
    SetSpacing(ui.GetSpacing()); // Update eraser tool spacing

    sf::Vector2f newPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), view) - m_Canvas.GetSprite().getPosition();

    if (m_LastPosition == newPosition)
        return;

    // Calculate the distance to be used for spacing
    float distanceMoved = std::sqrt(std::pow(newPosition.x - m_LastPosition.x, 2) +
        std::pow(newPosition.y - m_LastPosition.y, 2));

    // Calculate spacing distance for integer spacing value (0-500)
    float spacingDistance = 0.f;
    if (m_Spacing > 0) {
        spacingDistance = m_Thickness * (1.0f + m_Spacing * 0.05f);
    }

    m_AccumulatedDistance += distanceMoved;

    // Erase if accumulated distance reaches spacing distance or spacing is 0
    if (m_AccumulatedDistance >= spacingDistance || m_Spacing == 0)
    {
        sf::RenderStates states;
        states.blendMode = sf::BlendNone; // Deletes existing pixels

        // If spacing is active and enough distance has accumulated, erase as dots
        if (m_Spacing > 0)
        {
            // Erase as circles
            sf::CircleShape eraser(m_Thickness);
            eraser.setFillColor(sf::Color(0, 0, 0, 0)); // Tamamen þeffaf renk
            eraser.setPosition(newPosition - sf::Vector2f(m_Thickness, m_Thickness));
            m_Canvas.GetRenderTexture().draw(eraser, states);

            // Decrease accumulated distance as you move by multiples of the spacing distance
            while (m_AccumulatedDistance >= spacingDistance)
            {
                m_AccumulatedDistance -= spacingDistance;
            }
        }
        else // If Spacing is 0 erase continuously
        {
            // Erase a path between last position and current position
            for (int dx = -static_cast<int>(m_Thickness); dx <= static_cast<int>(m_Thickness); ++dx) {
                for (int dy = -static_cast<int>(m_Thickness); dy <= static_cast<int>(m_Thickness); ++dy) {
                    if (dx * dx + dy * dy <= m_Thickness * m_Thickness) { // Circular eraser effect
                        sf::Vector2f offset(static_cast<float>(dx), static_cast<float>(dy));

                        // Draw a transparent rectangle instead of a line
                        float length = std::sqrt(std::pow(newPosition.x - m_LastPosition.x, 2) +
                            std::pow(newPosition.y - m_LastPosition.y, 2));
                        sf::Angle angle = sf::degrees(std::atan2(newPosition.y - m_LastPosition.y,
                            newPosition.x - m_LastPosition.x) * 180.0f / 3.14159265f);

                        sf::RectangleShape rect(sf::Vector2f(length, 2.0f));
                        rect.setFillColor(sf::Color(0, 0, 0, 0));
                        rect.setOrigin({ 0, 1.0f });
                        rect.setPosition(m_LastPosition + offset);
                        rect.setRotation(angle);

                        m_Canvas.GetRenderTexture().draw(rect, states);
                    }
                }
            }

            // Additionally, draw a circle at the current position for better erasing
            sf::CircleShape eraser(m_Thickness);
            eraser.setFillColor(sf::Color(0, 0, 0, 0));
            eraser.setPosition(newPosition - sf::Vector2f(m_Thickness, m_Thickness));
            m_Canvas.GetRenderTexture().draw(eraser, states);

            // Reset accumulated distance for continuous erasing mode
            m_AccumulatedDistance = 0.f;
        }
    }

    m_LastPosition = newPosition;
}