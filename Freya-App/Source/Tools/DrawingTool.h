#pragma once
#include "Tool.h"
#include "Canvas.h"

namespace FRE {

    class DrawingTool : public Tool
    {
    public:
        DrawingTool(Canvas& canvas);

        // Overridden functions from the Tool class
        void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view) override;
        void Update(const sf::RenderWindow& window, const sf::View& view, UI& ui) override;

    private:
        bool m_IsDrawing = false;
    };

}