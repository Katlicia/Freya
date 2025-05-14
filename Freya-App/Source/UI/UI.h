#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

namespace FRE {
	class UI
	{
	public:
		UI(sf::RenderWindow& window);
		~UI();

		void Render(sf::RenderWindow& window);
		void Update(sf::Time deltaTime);
		void HandleEvent(const sf::Event& event);

	private:
		void ShowMainMenuBar();
		void ShowToolPanel();
		void ShowStatusBar();
		sf::RenderWindow& m_Window;

		// UI State variables
		const float SNAP_DISTANCE = 100.f;

		enum class AnchorType {
			Top,
			Bottom,
			Left,
			Right,
			None
		};

		AnchorType m_toolBarCurrentAnchor = AnchorType::None;
		ImVec2 m_toolPanelPos = ImVec2(0, 0);
		ImVec2 m_toolPanelSize = ImVec2(100.f, 500.f);
		bool m_isPanelSizeInitialized = false;
		bool m_isDragging = false;


	};
}

