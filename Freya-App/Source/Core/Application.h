#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>
#include <iostream>
#include "../Tools/DrawingTool.h"
#include "../Tools/Canvas.h"
#include "../UI/UI.h"
#include "LocalizationManager.h"

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
		std::unique_ptr<UI> m_UI;
		std::unique_ptr<LocalizationManager> m_LanguageManager;


		std::string m_Language;

		sf::View m_View;
		float m_InitialZoom;
		float m_MaxZoom;
		float m_MinZoom;
		sf::Vector2f m_InitialViewSize;

		sf::Color Grey = sf::Color(74, 76, 84);
		sf::Color m_BackgroundColor = Grey;
		bool m_IsRunning;
	};

}

