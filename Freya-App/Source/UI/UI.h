#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <Core/LocalizationManager.h>
#include "Tools/BrushType.h"

namespace FRE {
	class UI
	{
	public:
		UI(sf::RenderWindow& window, LocalizationManager languageManager);
		~UI();

		void Render(sf::RenderWindow& window);
		void Update(sf::Time deltaTime);
		void HandleEvent(const sf::Event& event);
		sf::Color GetColor();
		sf::Color ConvertToSFMLColor();

		float GetBrushSize();
		float GetSpacing();

		BrushType GetBrushType();

		bool CanDraw() const;

	private:
		void ShowMainMenuBar();
		void ShowToolPanel();
		void ShowStatusBar();
		void ShowColorPicker();
		void ShowToolBar();

		sf::RenderWindow& m_Window;
		LocalizationManager m_LanguageManager;

		float fontSize;
		float maxFontSize = 30.f;

		BrushType m_BrushType = BrushType::BRUSH;

		float m_Color[4] = { (float)0 / 255, (float)0 / 255, (float)0 / 255 , (float) 255 / 255 };
		int m_BrushSize = 1;
		int m_Spacing = 0;

		// UI Window management
		const float snapThreshold = 20.0f;  // Snap when within 20 pixels of the edge

	};
}

