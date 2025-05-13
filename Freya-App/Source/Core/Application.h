#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>
#include "../Tools/DrawingTool.h"
#include "../Tools/Canvas.h"
#include <iostream>

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

		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;

		std::unique_ptr<Canvas> m_Canvas;
		std::unique_ptr<DrawingTool> m_DrawingTool;

		sf::View m_View;

		sf::VideoMode desktop;

		bool m_IsRunning;
	};

}

