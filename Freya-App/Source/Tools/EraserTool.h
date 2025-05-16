#pragma once
#include "Tool.h"

namespace FRE {

    class EraserTool : public Tool
    {
    public:
        EraserTool(Canvas& canvas);

        // Overridden functions from the Tool class
        void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const sf::View& view) override;
        void Update(const sf::RenderWindow& window, const sf::View& view, UI& ui) override;

    private:
        bool m_IsErasing = false;
    };

}