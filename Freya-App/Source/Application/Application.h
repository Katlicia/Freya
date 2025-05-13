//#pragma once
//
//#include <SFML/Graphics.hpp>
//#include <imgui.h>
//#include <imgui-SFML.h>
//#include <nlohmann/json.hpp>
//
//namespace fr {
//	class Application
//	{
//	public:
//		// Singleton pattern for global application access
//		static Application& Get();
//
//		// Initialize the application
//		bool Init();
//
//		// Main application loop
//		void Run();
//
//		// Clean up and shutdown the application
//		void Shutdown();
//
//		// Get the main render window
//		sf::RenderWindow& GetWindow() { return m_Window; }
//
//		// Project management
//		void NewProject();
//		void SaveProject(const std::string& path);
//		void LoadProject(const std::string& path);
//
//		// Prevent copying of singleton
//		Application(const Application&) = delete;
//		Application& operator=(const Application&) = delete;
//	
//	private:
//		// Private constructor for singleton
//		Application();
//		~Application();
//
//		// Process events
//		void ProcessEvents();
//
//		// Update logic
//		void Update(float deltaTime);
//
//		// Render the application
//		void Render();
//
//		// Render ImGui interface
//		void RenderImGuiInterface();
//
//		// Main application window
//		sf::RenderWindow m_Window;
//		
//		// Clock for frame timing
//		sf::Clock m_DeltaClock;
//
//		// Current project data
//		nlohmann::json m_ProjectData;
//
//		// Application state
//		bool m_isRunning;
//	};
//}
//
//
