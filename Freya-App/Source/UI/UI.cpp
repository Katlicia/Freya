#include "UI.h"

using namespace FRE;

UI::UI(sf::RenderWindow& window, LocalizationManager languageManager) : m_Window(window), m_LanguageManager(languageManager)
{
	// Initialize ImGui-SFML
	if (!ImGui::SFML::Init(m_Window, false))
	{
		exit(EXIT_FAILURE);
	}

	ImGuiIO io = ImGui::GetIO();

	static const ImWchar turkish_chars[] = {
	0x20, 0x7F,       // Latin characters
	0x011E, 0x011F,   // Ð, ð
	0x0130, 0x0131,   // Ý, ý
	0x015E, 0x015F,   // Þ, þ
	0x00C7, 0x00E7,   // Ç, ç
	0x00D6, 0x00F6,   // Ö, ö
	0x00DC, 0x00FC,   // Ü, ü
	0, 0              // end
	};

	ImFontConfig font_cfg;
	font_cfg.OversampleH = 2;
	font_cfg.OversampleV = 2;

	fontSize = 20.f;

	ImFont* font1 = io.Fonts->AddFontFromFileTTF("Source\\Assets\\Poppins-Regular.ttf", fontSize, &font_cfg, turkish_chars);

	ImGui::SFML::UpdateFontTexture();
}

UI::~UI()
{
	ImGui::SFML::Shutdown();
}

void UI::Update(sf::Time deltaTime) {
	ImGui::SFML::Update(m_Window, deltaTime);
}

void UI::HandleEvent(const sf::Event& event) {
	ImGui::SFML::ProcessEvent(m_Window, event);

	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::B)
		{
			m_BrushType = BrushType::BRUSH;
		}

		if (keyPressed->scancode == sf::Keyboard::Scancode::E)
		{
			m_BrushType = BrushType::ERASER;
		}
	}
}

void UI::Render(sf::RenderWindow& window) {
	//ShowMainMenuBar();
	ShowToolPanel();
	//ShowStatusBar();
	ShowColorPicker();
	ShowToolBar();
	ImGui::SFML::Render(window);
}

void UI::ShowMainMenuBar() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit")) {
				// Exit application
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Tool Panel", nullptr);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void UI::ShowStatusBar() {
	
	float statusBarHeight = 20.0f;

	ImGui::SetNextWindowPos(ImVec2(-5, -5));
	ImGui::SetNextWindowSize(ImVec2(100, statusBarHeight));
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	//ImGui::GetFont()->Scale = 1.0f;
	//ImGui::PushFont(ImGui::GetFont());
	

	ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
	ImGui::Text("FPS:%.f", ImGui::GetIO().Framerate);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	//ImGui::PopFont();

	ImGui::End();
}

void UI::ShowColorPicker() {
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 windowSize = ImVec2(340, 305);
	const ImVec2 desiredPos = ImVec2(0.0f, io.DisplaySize.y - windowSize.y);
	ImGui::SetNextWindowPos(desiredPos, ImGuiCond_FirstUseEver);	
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // Corner roundness
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);        // Border thickness
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(120, 120, 120, 100));  // Border color

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255, 0, 0, 255));      // Input boxes background
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 255, 0, 255));       // Button color
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 255, 0, 255));       // Title hover colors
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255, 255, 0, 255));    // Border
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 255, 0, 255));    // Border
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 255, 255));    // Border

    ImGui::Begin(m_LanguageManager.Get("color_picker_title").c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);


    // Snap logic:
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();

    bool changed = false;

    if (pos.x <= snapThreshold) {
        pos.x = 0.0f;
        changed = true;
    }
    else if (pos.x + size.x >= io.DisplaySize.x - snapThreshold) {
        pos.x = io.DisplaySize.x - size.x;
        changed = true;
    }

    if (pos.y <= snapThreshold) {
        pos.y = 0.0f;
        changed = true;
    }
    else if (pos.y + size.y >= io.DisplaySize.y - snapThreshold) {
        pos.y = io.DisplaySize.y - size.y;
        changed = true;
    }

    if (changed) {
        ImGui::SetWindowPos(pos);
    }

    ImGui::ColorPicker4("##picker_popup", m_Color, ImGuiColorEditFlags_AlphaBar);
    ImGui::End();
	ImGui::PopStyleColor(7);  // Border
	ImGui::PopStyleVar(2);   // Rounding ve BorderSize
}

void UI::ShowToolPanel() {
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_Window.getSize().x, fontSize * 3), ImGuiCond_Always);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("Toolbar", nullptr, flags);

	ImGui::PushItemWidth(150);

	ImVec2 currentCurPos = ImGui::GetCursorPos();

	// Brush Size
	ImGui::Text(m_LanguageManager.Get("brush_size").c_str());
	ImGui::SetCursorPos(ImVec2(currentCurPos.x, currentCurPos.y + fontSize));
	ImGui::SliderInt("##BrushSize", &m_BrushSize, 1, 100);

	ImVec2 pos2(currentCurPos.x + 170, currentCurPos.y);
	ImGui::SetCursorPos(pos2);

	// Spacing (0.0 - 500.0)
	ImGui::Text(m_LanguageManager.Get("spacing").c_str());
	ImGui::SetCursorPos(ImVec2(pos2.x, pos2.y + fontSize));
	ImGui::SliderInt("##Spacing", &m_Spacing, 0, 500);

	if (m_BrushType != BrushType::ERASER)
	{
		ImGui::SetCursorPos(ImVec2(currentCurPos.x + 340, pos2.y));
		// Opacity (0.0 - 1.0)
		ImGui::Text(m_LanguageManager.Get("opacity").c_str());
		ImGui::SetCursorPos(ImVec2(currentCurPos.x + 340, pos2.y + fontSize));
		ImGui::SliderFloat("##Opacity", &m_Color[3], 0.0f, 1.f);

		// Color
		ImGui::SetCursorPos(ImVec2(currentCurPos.x + 510, pos2.y));

		ImGui::Text(m_LanguageManager.Get("color").c_str());
		ImGui::SetCursorPos(ImVec2(currentCurPos.x + 510, pos2.y + fontSize));
		ImGui::ColorEdit4("##Color", m_Color, ImGuiColorEditFlags_NoInputs);
	}

	ImGui::PopItemWidth();

	ImGui::End();
}

void UI::ShowToolBar() {
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 minSize = ImVec2(fontSize * 4.5f, 300); // Minimum size for the toolbar
    const ImVec2 maxSize = ImVec2(200, 600); // Maximum size for the toolbar
    const ImVec2 defaultSize = ImVec2(fontSize * 4.5f, 300); // Default window size

    const float snapThreshold = 20.0f; // Snap threshold in pixels
    
    // Position on the right side of the screen
    const ImVec2 desiredPos = ImVec2(io.DisplaySize.x - defaultSize.x, 
                                    (io.DisplaySize.y - defaultSize.y) / 2);
    ImGui::SetNextWindowPos(desiredPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
    
    // Apply styling similar to the color picker
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // Corner roundness
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);        // Border thickness
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(120, 120, 120, 100));  // Border color
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255, 0, 0, 255));     // Input box background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 255, 0, 255));      // Button color
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 255, 0, 255));      // Header hover colors
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255, 255, 0, 255));    // Border
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 255, 0, 255));     // Title background
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 255, 255)); // Active title background
    
	

    // Create window with resize and collapse flags
    ImGui::Begin(m_LanguageManager.Get("tool_title").c_str(), nullptr,
                ImGuiWindowFlags_NoSavedSettings);
    
	ImVec2 available = ImGui::GetContentRegionAvail();
	float buttonWidth = available.x * 0.9f;
	float buttonHeight = available.y / 10.0f;


    // Snap logic:
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    bool changed = false;
    
    // Snap to screen edges
    if (pos.x <= snapThreshold) {
        pos.x = 0.0f;
        changed = true;
    }
    else if (pos.x + size.x >= io.DisplaySize.x - snapThreshold) {
        pos.x = io.DisplaySize.x - size.x;
        changed = true;
    }
    
    if (pos.y <= snapThreshold) {
        pos.y = 0.0f;
        changed = true;
    }
    else if (pos.y + size.y >= io.DisplaySize.y - snapThreshold) {
        pos.y = io.DisplaySize.y - size.y;
        changed = true;
    }
    
    if (changed) {
        ImGui::SetWindowPos(pos);
    }
    
    // Example buttons
	if (ImGui::Button(m_LanguageManager.Get("brush").c_str(), ImVec2(buttonWidth, buttonHeight))) {
		m_BrushType = BrushType::BRUSH;
    }

	//if (ImGui::IsItemHovered()) {
	//	ImGui::SetTooltip("Tooltip example.");
	//}
    
    if (ImGui::Button(m_LanguageManager.Get("eraser").c_str(), ImVec2(buttonWidth, buttonHeight))) {
		m_BrushType = BrushType::ERASER;
    }
    
    if (ImGui::Button("Tool 3", ImVec2(fontSize * 3, fontSize + fontSize / 3))) {
        // Code for Tool 3
    }
    
    ImGui::End();
    
    // Pop all the styling
    ImGui::PopStyleColor(7);  // 7 colors were pushed
    ImGui::PopStyleVar(2);    // 2 style variables were pushed
}



sf::Color UI::GetColor() {
   return ConvertToSFMLColor();
}

float UI::GetBrushSize() {
	return (float)m_BrushSize;
}

float UI::GetSpacing() {
	return m_Spacing;
}


BrushType UI::GetBrushType() {
	return m_BrushType;
}

sf::Color UI::ConvertToSFMLColor() {
	return sf::Color(
		static_cast<std::uint8_t>(m_Color[0] * 255),
		static_cast<std::uint8_t>(m_Color[1] * 255),
		static_cast<std::uint8_t>(m_Color[2] * 255),
		static_cast<std::uint8_t>(m_Color[3] * 255)
	);
}

bool UI::CanDraw() const
{
	// Eðer ImGui bir item üzerinde aktifse veya bir pencereyi hover ediyorsa, çizim yapýlmamalý
	return !(ImGui::GetIO().WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));
}
