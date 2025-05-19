#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>
#include <iostream>
#include "Tools/DrawingTool.h"
#include "Tools/Canvas.h"
#include "Tools/EraserTool.h"
#include "UI/UI.h"
#include "LocalizationManager.h"
#include "Tools/BrushType.h"
#include "SettingsManager.h"

class Canvas;
class DrawingTool;

namespace FRE {

	class Application
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		void ProcessEvents();
		void Update(sf::Time deltaTime);
		void Render();

		sf::VideoMode desktop;

		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;

		
		std::unique_ptr<Canvas> m_Canvas;
		std::unique_ptr<DrawingTool> m_DrawingTool;
		std::unique_ptr<EraserTool> m_EraserTool;
		std::unique_ptr<UI> m_UI;
		std::unique_ptr<LocalizationManager> m_LanguageManager;
		std::unique_ptr<SettingsManager> m_SettingsManager;
		
		bool m_draw = true;

		BrushType m_LastBrush;
		BrushType m_CurrentBrush;

		Tool* m_ActiveTool = m_DrawingTool.get();

		std::string m_Language;

		sf::View m_View;
		float m_InitialZoom;
		float m_MaxZoom = 3.f;
		float m_MinZoom = 0.01f;
		float m_CurrentZoom = 1.f;
		sf::Vector2f m_InitialViewSize;

		bool m_IsPanning = false;
		sf::Vector2i m_PanStartMousePos;
		sf::Vector2f m_PanStartViewCenter;


		sf::Color Grey = sf::Color(74, 76, 84);
		sf::Color m_BackgroundColor = Grey;
		bool m_IsRunning;
	};

}

