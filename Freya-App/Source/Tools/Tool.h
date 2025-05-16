#pragma once
#include <SFML/Graphics.hpp>
#include "Canvas.h"
#include "UI/UI.h"

namespace FRE {

    // Base Tool class - abstract base class for all tools
    class Tool
    {
    public:
        Tool(Canvas& canvas) : m_Canvas(canvas) {}
        virtual ~Tool() = default;

        // Basic functions that must be implemented for each tool
        virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view) = 0;
        virtual void Update(const sf::RenderWindow& window, const sf::View& view, UI& ui) = 0;

        // Common features that can be found in all tools
        virtual void SetColor(sf::Color color) { m_Color = color; }
        virtual void SetThickness(float thickness) {
            m_Thickness = std::clamp(thickness, 1.f, m_MaxThickness);
        }
        virtual float GetThickness() { return m_Thickness; }
        virtual void SetSpacing(int spacing) {
            m_Spacing = std::max(0, spacing);
        }

    protected:
        Canvas& m_Canvas;
        sf::Color m_Color;
        float m_Thickness = 1.f;
        float m_MaxThickness = 100.f;
        int m_Spacing = 0;
        bool m_IsActive = false;
        sf::Vector2f m_LastPosition;
        float m_AccumulatedDistance = 0.f;
    };

}