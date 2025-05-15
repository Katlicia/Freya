#include "Application.h"

using namespace FRE;

Application::Application() :
	m_Canvas(nullptr),
	m_DrawingTool(nullptr),
	m_UI(nullptr),
	m_IsRunning(true)
{
	// Get monitor resolution
	desktop = sf::VideoMode::getDesktopMode();

	m_Window = sf::RenderWindow(sf::VideoMode({ desktop.size.x, desktop.size.y }),
		"Freya", sf::Style::Default);

	m_View = sf::View(sf::FloatRect({ 0.f, 0.f },
		{ static_cast<float>(desktop.size.x), static_cast<float>(desktop.size.y) }));

	if (desktop.size.x < 800 || desktop.size.y < 600)
	{
		std::cerr << "Warning: desktop resolution too small or invalid. Falling back.\n";
		desktop = sf::VideoMode({ 1920, 1080 });
	}

	m_Window.setVerticalSyncEnabled(true);

	m_LanguageManager = std::make_unique<LocalizationManager>();
	m_Language = m_LanguageManager->GetSystemLanguage();
	if (!m_LanguageManager->Load("Source/Assets/lang_" + m_Language + ".json")) {
		std::cerr << "Error: Could not load language file: lang_" + m_Language + ".json" << std::endl;
		return;
	}

	m_UI = std::make_unique<UI>(m_Window, *m_LanguageManager);
	m_Canvas = std::make_unique<Canvas>();
	m_DrawingTool = std::make_unique<DrawingTool>(*m_Canvas);
	m_DrawingTool->SetColor(m_UI->GetColor());
	
	m_InitialZoom = 1.f;
	m_MaxZoom = 1.f;
	m_MinZoom = 0.1f;
	m_InitialViewSize = m_View.getSize();

}

Application::~Application()
{

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
			m_Window.close();
			/*m_IsRunning = false;*/ // (Crashes) FIX BUG

		m_DrawingTool->HandleEvent(*event, m_Window, m_View); // Handle input events for drawing tool

		if (const auto* resized = event->getIf<sf::Event::Resized>())
		{
			sf::Vector2u newSize = resized->size;
			sf::Vector2f currentCenter = m_View.getCenter();
			m_View.setSize({ float(newSize.x), float(newSize.y) });
			m_View.setCenter(currentCenter);
			m_Window.setView(m_View);
		}

		m_UI->HandleEvent(*event); // Pass event to UI

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			if (keyPressed->scancode == sf::Keyboard::Scancode::Z)
			{
				m_DrawingTool->SetThickness(m_DrawingTool->GetThickness() + 1);
			}
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			if (keyPressed->scancode == sf::Keyboard::Scancode::X)
			{
				m_DrawingTool->SetThickness(m_DrawingTool->GetThickness() - 1);
			}
		}

		//if (const auto* wheelScroll = event->getIf<sf::Event::MouseWheelScrolled>())
		//{
		//	if (wheelScroll->wheel == sf::Mouse::Wheel::Vertical)
		//	{
		//		// Calculate current zoom level based on the initial view size
		//		float currentZoom = m_View.getSize().x / m_InitialViewSize.x;

		//		if (wheelScroll->delta > 0)
		//		{
		//			if (currentZoom > m_MinZoom)
		//			{
		//				float zoomFactor = 0.9f;
		//				if (currentZoom * zoomFactor < m_MinZoom)
		//				{
		//					zoomFactor = m_MinZoom / currentZoom;
		//				}
		//				m_View.zoom(zoomFactor);
		//			}
		//		}
		//		else if (wheelScroll->delta < 0)
		//		{

		//			if (currentZoom < m_MaxZoom)
		//			{
		//				float zoomFactor = 1.1f;
		//				if (currentZoom * zoomFactor > m_MaxZoom)
		//				{
		//					zoomFactor = m_MaxZoom / currentZoom;
		//				}
		//				m_View.zoom(zoomFactor);
		//			}
		//		}
		//	}
		//}
	}
}

void Application::Update(sf::Time deltaTime)
{
	m_UI->Update(deltaTime);
	m_DrawingTool->Update(m_Window, m_View);
	m_Canvas->Update(m_Window, m_View);
	m_DrawingTool->SetColor(m_UI->GetColor()); // Update drawing tool color
	m_DrawingTool->SetThickness(m_UI->GetBrushSize()); // Update drawing tool thickness
}

void Application::Render()
{
	m_Window.clear(m_BackgroundColor);
	m_Window.setView(m_View);
	m_Window.draw(m_Canvas->getSprite());
	
	// Render UI
	m_Window.setView(m_Window.getDefaultView());
	m_UI->Render(m_Window);

	m_Window.display();
}