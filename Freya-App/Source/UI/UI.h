#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <Core/LocalizationManager.h>
#include "Tools/BrushType.h"
#include "Tools/Canvas.h"
#include <nfd.h>
#include <filesystem>

namespace FRE {
	class UI
	{
	public:
		UI(sf::RenderWindow& window, LocalizationManager languageManager);
		~UI();

		void Render(sf::RenderWindow& window);
		void Update(sf::Time deltaTime);
		void HandleEvent(const sf::Event& event);
		
		sf::Color GetColor();
		sf::Color ConvertToSFMLColor();

		void ResizeCanvas(int width, int height);

		float GetBrushSize();
		float GetSpacing();

		void SetCanvas(Canvas* canvas);

		BrushType GetBrushType();

		bool CanDraw() const;

		int GetFontSize();
		void SetFontSize(int size);

		std::string GetLanguage();
		void SetLanguage(const std::string& language);

		void ExportImage();

		void ShowNotification(const std::string& message, bool isError = false);
		void RenderNotifications();

		std::string ShowFileDialog();
		std::string ShowImageDialog();

	private:
		void ShowMainMenuBar();
		void ShowToolPanel();
		void ShowStatusBar();
		void ShowColorPicker();
		void ShowToolBar();

		sf::RenderWindow& m_Window;
		LocalizationManager m_LanguageManager;
		Canvas* m_Canvas = nullptr;
		unsigned int m_CanvasWidth;
		unsigned int m_CanvasHeight;
		int m_ResizeWidth;
		int m_ResizeHeight;

		int fontSize = 18;
		int maxFontSize = 30;
		int minimumFontSize = 18;

		bool m_pendingFontSizeChange = false;

		std::string m_Language;
		std::string currentLangLabel;

		BrushType m_BrushType = BrushType::BRUSH;

		float m_Color[4] = { (float)0 / 255, (float)0 / 255, (float)0 / 255 , (float) 255 / 255 };
		int m_BrushSize = 1;
		int m_Spacing = 0;

		// UI Window management
		const float snapThreshold = 20.0f;  // Snap when within 20 pixels of the edge
		bool openSettings = false;
		bool openExportDialog = false;
		bool openResizeDialog = false;

		// Export settings
		std::string m_ExportFilePath = "";
		std::string m_ExportFileName = "";
		int m_SelectedFormat = 0; // 0: PNG, 1: JPEG, 2: BMP

		// Notifs
		struct Notification {
			std::string message;
			float timeRemaining;
			bool isError;
		};
		std::vector<Notification> m_Notifications;

	};
}

