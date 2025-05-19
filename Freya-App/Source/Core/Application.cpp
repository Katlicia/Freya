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
	m_SettingsManager = std::make_unique<SettingsManager>();
	m_UI->SetCanvas(&*m_Canvas);
	
	if (!std::filesystem::exists("settings.ini")) {
		std::cout << "First run detected. Creating default settings.ini\n";
		m_SettingsManager->Set("UserLanguage", m_Language);
		m_SettingsManager->Set("UserFontSize", std::to_string(m_UI->GetFontSize()));
		m_SettingsManager->Save("settings.ini");
	}

	if (!m_SettingsManager->Load("settings.ini")) {
		std::cerr << "Warning: Could not load settings.ini\n";
	}

	m_UI->SetLanguage(m_SettingsManager->Get("UserLanguage", m_Language));
	m_UI->SetFontSize(std::stof(m_SettingsManager->Get("UserFontSize", std::to_string(m_UI->GetFontSize()))));


	m_LastBrush = m_UI->GetBrushType();
	m_ActiveTool = m_DrawingTool.get();
	m_InitialViewSize = m_View.getSize();
}

Application::~Application()
{
	m_SettingsManager->Set("UserLanguage", m_UI->GetLanguage());
	m_SettingsManager->Set("UserFontSize", std::to_string(m_UI->GetFontSize()));
	m_SettingsManager->Save("settings.ini");
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
				//m_Canvas->ExportToPNG("test.png");
			}


			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				//if (keyPressed->scancode == sf::Keyboard::Scancode::Y)
				//{
				//	m_UI->SetFontSize(m_UI->GetFontSize()+1);
				//}

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

		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
		{
			if (mousePressed->button == sf::Mouse::Button::Middle)
			{
				m_IsPanning = true;
				m_PanStartMousePos = sf::Mouse::getPosition(m_Window);
				m_PanStartViewCenter = m_View.getCenter();
			}
		}

		if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
		{
			if (mouseReleased->button == sf::Mouse::Button::Middle)
			{
				m_IsPanning = false;
			}
		}


		if (const auto* wheelScroll = event->getIf<sf::Event::MouseWheelScrolled>())
		{
			if (wheelScroll->wheel == sf::Mouse::Wheel::Vertical)
			{
				// Calculate current zoom level based on the initial view size
				//float currentZoom = m_View.getSize().x / m_InitialViewSize.x;

				if (wheelScroll->delta > 0)
				{
					if (m_CurrentZoom > m_MinZoom)
					{
						float zoomFactor = 0.9f;
						if (m_CurrentZoom * zoomFactor < m_MinZoom)
						{
							zoomFactor = m_MinZoom / m_CurrentZoom;
						}
						m_View.zoom(zoomFactor);
					}
				}
				else if (wheelScroll->delta < 0)
				{
					if (m_CurrentZoom < m_MaxZoom)
					{
						float zoomFactor = 1.1f;
						if (m_CurrentZoom * zoomFactor > m_MaxZoom)
						{
							zoomFactor = m_MaxZoom / m_CurrentZoom;
						}
						m_View.zoom(zoomFactor);
					}
				}
			}
		}
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

	m_CurrentZoom = m_View.getSize().x / m_InitialViewSize.x;

	if (m_UI->GetZoomIn())
	{
		if (m_CurrentZoom > m_MinZoom)
		{
			m_View.zoom(0.75f);
			m_UI->SetZoomIn(false);
		}
	}
	if (m_UI->GetZoomOut())
	{
		if (m_CurrentZoom < m_MaxZoom)
		{
			m_View.zoom(1.25f);
			m_UI->SetZoomOut(false);
		}
	}

	if (m_IsPanning)
	{
		sf::Vector2i currentMousePos = sf::Mouse::getPosition(m_Window);
		sf::Vector2i delta = m_PanStartMousePos - currentMousePos;

		// Scale pan speed according to zoom level
		float panScale = m_View.getSize().x / m_InitialViewSize.x;

		m_View.setCenter(m_PanStartViewCenter + sf::Vector2f(delta.x, delta.y) * panScale);
	}

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
	m_Window.setTitle("Freya - " + m_UI->GetOpenedProjectName());
}