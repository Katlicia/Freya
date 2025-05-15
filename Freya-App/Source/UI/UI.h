#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <Core/LocalizationManager.h>

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

	private:
		void ShowMainMenuBar();
		void ShowToolPanel();
		void ShowStatusBar();
		void ShowColorPicker();
		sf::RenderWindow& m_Window;
		LocalizationManager m_LanguageManager;

		// UI State variables
		const float SNAP_DISTANCE = 100.f;

		enum class AnchorType {
			Top,
			Bottom,
			Left,
			Right,
			None
		};

		float m_Color[4] = { (float)0 / 255, (float)0 / 255, (float)0 / 255 , (float) 255 / 255 };
		float m_OriginalColor[4] = { (float)0 / 255, (float)0 / 255, (float)0 / 255 , (float) 255 / 255 };

		AnchorType m_toolBarCurrentAnchor = AnchorType::Right;
		ImVec2 m_toolPanelPos = ImVec2(0, 0);
		ImVec2 m_toolPanelSize = ImVec2(100.f, 500.f);
		bool m_isPanelSizeInitialized = false;
		bool m_isDragging = false;

		// Tool Components

		int m_BrushSize = 1;
		float m_Hardness = 1.f;
		float m_Spacing = 1.f;
	};
}

