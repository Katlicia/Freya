#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>
#include <iostream>
#include "../Tools/DrawingTool.h"
#include "../Tools/Canvas.h"
#include "../UI/UI.h"

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

		sf::View m_View;

		sf::Color Grey = sf::Color(74, 76, 84);
		sf::Color m_BackgroundColor = Grey;
		bool m_IsRunning;
	};

}

