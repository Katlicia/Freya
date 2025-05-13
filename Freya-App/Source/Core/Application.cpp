#include "Application.h"

using namespace FRE;

Application::Application() : m_Window(sf::VideoMode({ sf::VideoMode::getDesktopMode().size.x, sf::VideoMode::getDesktopMode().size.y }), "Freya", sf::Style::Default),
  m_Canvas(nullptr),
  m_DrawingTool(nullptr),
  m_IsRunning(true)
{
	m_Window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(m_Window);
	// SMART PTR Kullan
	m_Canvas = std::make_unique<Canvas>();
	m_DrawingTool = std::make_unique<DrawingTool>(*m_Canvas);
}

Application::~Application()
{
	ImGui::SFML::Shutdown();
}


void Application::Run()
{
	while (m_IsRunning && m_Window.isOpen())
	{
		sf::Time deltaTime = m_DeltaClock.restart();

		ProcessEvents();
		Update(deltaTime);
		Render();
	}
}

void Application::ProcessEvents()
{
	while (const std::optional event = m_Window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			m_IsRunning = false;
	}
}

void Application::Update(sf::Time deltaTime)
{
	ImGui::SFML::Update(m_Window, deltaTime);

	m_DrawingTool->Update(m_Window); // giriþ olaylarýný buraya gönderiyoruz
}

void Application::Render()
{
	m_Window.clear(sf::Color::White);

	m_Canvas->Draw(m_Window); // sadece canvas çiziyor
	ImGui::SFML::Render(m_Window);

	m_Window.display();
}