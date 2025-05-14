#include "Application.h"

using namespace FRE;

Application::Application() :
	m_Canvas(nullptr),
	m_DrawingTool(nullptr),
	m_UI(nullptr),
	m_IsRunning(true)
{
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
			/*m_IsRunning = false;*/

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
	}
}

void Application::Update(sf::Time deltaTime)
{
	m_UI->Update(deltaTime); // Update UI
	m_DrawingTool->Update(m_Window, m_View); // Input Events
	m_Canvas->Update(m_Window, m_View); // Update canvas
	m_DrawingTool->SetColor(m_UI->GetColor()); // Update drawing tool color
}

void Application::Render()
{
	m_Window.clear(m_BackgroundColor);
	m_Window.draw(m_Canvas->getSprite()); // Draw canvas as sprite
	// Render UI
	m_UI->Render(m_Window);

	m_Window.display();
}