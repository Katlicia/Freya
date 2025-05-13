//#include "Application.h"
//#include <iostream>
//#include <fstream>
//
//namespace fr {
//	// Singleton instance
//	Application& Application::Get()
//	{
//		static Application instance;
//		return instance;
//	}
//
//	Application::Application()
//		: m_Window(sf::VideoMode(sf::VideoMode::getDesktopMode().size.x, sf::VideoMode::getDesktopMode().size.y), "Freya"),
//		m_IsRunning(false) {
//		// Default constructor
//	}
//
//	Application::~Application()
//	{
//		Shutdown();
//	}
//
//	bool Application::Init() {
//		// Set frame rate limit
//		m_Window.setVerticalSyncEnabled(true);
//
//		// Initilize ImGui
//		if (!ImGui::SFML::Init(m_Window)) {
//			std::cerr << "Failed to initialize ImGui" << std::endl;
//			return false;
//		}
//
//		// Initialize default project data
//		m_ProjectData = {
//			{"name", "Untitled Project"},
//			{"version", "0.1.0"},
//			{"canvas",
//				{
//					{"width", 800},
//					{"height", 600}
//				}
//			}
//		};
//
//		m_IsRunning = true;
//		return true;
//	}
//
//	void Application::Run() {
//		// Ensure initialization was successful
//		if (!m_IsRunning) {
//			if (!Initialize()) {
//				return;
//			}
//		}
//
//		// Main application loop
//		while (m_Window.isOpen() && m_IsRunning) {
//			// Calculate delta time
//			float deltaTime = m_DeltaClock.restart().asSeconds();
//
//			// Process events
//			ProcessEvents();
//
//			// Update logic
//			Update(deltaTime);
//
//			// Render the application
//			Render();
//		}
//	}
//
//	void Application::ProcessEvents() {
//		while (const std::optional event = m_Window.pollEvent())
//		{
//			ImGui::SFML::ProcessEvent(m_Window, *event);
//			if (event->is<sf::Event::Closed>())
//				m_Window.close();
//				m_IsRunning = false;
//			if (event->is<sf::Event::Resized()>)
//				m_Window.setView(sf::View(sf::FloatRect(0.f, 0.f,
//					static_cast<float>(event->size.width),
//					static_cast<float>(event->size.height))));
//		}
//	}
//
//	void Application::Update(float deltaTime) {
//		// Update ImGui
//		ImGui::SFML::Update(m_Window, sf::seconds(deltaTime));
//	}
//
//
//	void Application::Render() {
//		// Clear the window
//		m_Window.clear(sf::Color(50, 50, 50));
//		// Render ImGui interface
//		RenderImGuiInterface();
//		// Display the window contents
//		m_Window.display();
//	}
//
//	void Application::RenderImGuiInterface() {
//		// Main menu bar 
//		if (ImGui::BeginMainMenuBar()) {
//			if (ImGui::BeginMenu("File")) {
//				if (ImGui::MenuItem("New Project")) {
//					NewProject();
//				}
//				if (ImGui::MenuItem("Save Project")) {
//					SaveProject("project.json");
//				}
//				if (ImGui::MenuItem("Load Project")) {
//					LoadProject("project.json");
//				}
//				ImGui::Seperator();
//				if (ImGui::MenuItem("Exit")) {
//					m_IsRunning = false;
//				}
//				ImGui::EndMenu();
//			}
//
//			if (ImGui::BeginMenu("Edit")) {
//				// Future edit menu items
//				ImGui::EndMenu();
//			}
//			ImGui::EndMainMenuBar();
//		}
//
//		// Project information window
//		ImGui::Begin("Project Info");
//		ImGui::Text("Project Name: %s",
//			m_ProjectData["name"].get<std::string>().c_str());
//		ImGui::Text("Canvas Size: %dx%d",
//			m_ProjectData["canvas"]["width"].get<int>(),
//			m_ProjectData["canvas"]["height"].get<int>());
//		ImGui::End();
//
//		// Render ImGui
//		ImGui::SFML::Render(m_Window);
//	}
//
//	void Application::NewProject() {
//		// Reset project data to default
//		m_ProjectData = {
//			{"name", "Untitled Project"},
//			{"version", "0.1.0"},
//			{"canvas",
//				{
//					{"width", 800},
//					{"height", 600}
//				}
//			}
//		};
//	}
//
//	void Application::SaveProject(const std::string& filepath) {
//		try {
//			std::ofstream file(filepath);
//			if (!file.is_open()) {
//				std::cerr << "Failed to open file for saving: " << filepath << std::endl;
//				return;
//			}
//
//			file << m_ProjectData.dump(4);  // Pretty print with 4 spaces
//			file.close();
//
//			std::cout << "Project saved successfully to " << filepath << std::endl;
//		}
//		catch (const std::exception& e) {
//			std::cerr << "Error saving project: " << e.what() << std::endl;
//		}
//	}
//
//	void Application::LoadProject(const std::string& filepath) {
//		try {
//			std::ifstream file(filepath);
//			if (!file.is_open()) {
//				std::cerr << "Failed to open file for loading: " << filepath << std::endl;
//				return;
//			}
//
//			m_ProjectData = nlohmann::json::parse(file);
//			file.close();
//
//			std::cout << "Project loaded successfully from " << filepath << std::endl;
//		}
//		catch (const std::exception& e) {
//			std::cerr << "Error loading project: " << e.what() << std::endl;
//		}
//	}
//
//	void Application::Shutdown() {
//		// Cleanup ImGui
//		ImGui::SFML::Shutdown();
//
//		// Close window if still open
//		if (m_Window.isOpen()) {
//			m_Window.close();
//		}
//	}
//
//}