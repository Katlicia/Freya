#include "Application.h"

using namespace FRE;

Application::Application() :
	m_Canvas(nullptr),
	m_DrawingTool(nullptr),
	m_EraserTool(nullptr),
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
	m_EraserTool = std::make_unique<EraserTool>(*m_Canvas);
	m_LastBrush = m_UI->GetBrushType();
	m_ActiveTool = m_DrawingTool.get();
	
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

		if (const auto* resized = event->getIf<sf::Event::Resized>())
		{
			sf::Vector2u newSize = resized->size;
			sf::Vector2f currentCenter = m_View.getCenter();
			m_View.setSize({ float(newSize.x), float(newSize.y) });
			m_View.setCenter(currentCenter);
			m_Window.setView(m_View);
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			if (keyPressed->scancode == sf::Keyboard::Scancode::X)
			{
				m_Canvas->ExportToPNG("test.png");
			}
		}

		if (m_LastBrush != m_CurrentBrush) {
			switch (m_CurrentBrush) {
			case BrushType::BRUSH:
				m_ActiveTool = m_DrawingTool.get();
				break;
			case BrushType::ERASER:
				m_ActiveTool = m_EraserTool.get();
				break;
			}
			m_LastBrush = m_CurrentBrush;
		}

		if (m_UI->CanDraw()) // If user is interacting with ImGUI window cancel drawing
		{
			m_ActiveTool->HandleEvent(*event, m_Window, m_View); // Handle input events for active tool
		}


		m_UI->HandleEvent(*event); // Pass event to UI

		//if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		//{
		//	if (keyPressed->scancode == sf::Keyboard::Scancode::E)
		//	{
		//		m_draw = !m_draw; // Toggle between drawing and erasing
		//		/*m_EraserTool->SetThickness(50.f);*/
		//	}
		//}


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
	m_CurrentBrush = m_UI->GetBrushType();

	if (m_ActiveTool) {
		m_ActiveTool->SetThickness(m_UI->GetBrushSize());
		m_ActiveTool->SetSpacing(m_UI->GetSpacing());
		if (m_CurrentBrush == BrushType::BRUSH)
			m_ActiveTool->SetColor(m_UI->GetColor());
	}

	if (m_ActiveTool)
		m_ActiveTool->Update(m_Window, m_View, *m_UI);

	m_Canvas->Update(m_Window, m_View);

}

void Application::Render()
{
	m_Window.clear(m_BackgroundColor);
	m_Window.setView(m_View);
	m_Canvas->DrawTransparencyPattern(m_Window);
	m_Window.draw(m_Canvas->GetSprite());
	
	// Render UI
	m_Window.setView(m_Window.getDefaultView());
	m_UI->Render(m_Window);

	m_Window.display();
}