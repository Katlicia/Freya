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
	0x00B0, 0x00B0,   // ° (degree sign)
	0, 0              // end
	};

	static const ImWchar* glyph_ranges = io.Fonts->GetGlyphRangesDefault(); // Default glyph ranges

	ImFontConfig font_cfg;
	font_cfg.OversampleH = 2;
	font_cfg.OversampleV = 2;

    if (m_LanguageManager.GetSystemLanguage() == "tr")
	{
		glyph_ranges  = turkish_chars;
	}


	ImFont* font1 = io.Fonts->AddFontFromFileTTF("Source\\Assets\\Poppins-Regular.ttf", (float)fontSize, &font_cfg, glyph_ranges);

	ImGui::SFML::UpdateFontTexture();
}

UI::~UI()
{
	ImGui::SFML::Shutdown();
}

void UI::Update(sf::Time deltaTime) {
    if (m_pendingFontSizeChange)
    {
        SetFontSize(fontSize);
        m_pendingFontSizeChange = false;
    }

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
	ShowMainMenuBar();
	ShowToolPanel();
	ShowColorPicker();
	ShowToolBar();
	ImGui::SFML::Render(window);
}

void UI::ShowMainMenuBar() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(m_LanguageManager.Get("file").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("new").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("open").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("save").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("import").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("export").c_str())) 
            {
                SetExport(true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(m_LanguageManager.Get("edit").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("undo").c_str())) { /* ... */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(m_LanguageManager.Get("view").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("zoomin").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("zoomout").c_str())) { /* ... */ }
            ImGui::EndMenu();
        }
        }

        if (ImGui::BeginMenu(m_LanguageManager.Get("display").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("resize").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("horizontal_turn").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("vertical_turn").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("90_clock_rotate").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("90_counterclock_rotate").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("180_rotate").c_str())) { /* ... */ }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(m_LanguageManager.Get("layers").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("add_layer").c_str())) { /* ... */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(m_LanguageManager.Get("effects").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("blur").c_str())) { /* ... */ }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem(m_LanguageManager.Get("settings").c_str()))
        {
            openSettings = true;
        }

        ImGui::EndMainMenuBar();

        if (openSettings) {
			ImGui::OpenPopup("SettingsPopup");
            openSettings = false;
        }

        if (ImGui::BeginPopup("SettingsPopup"))
        {
            ImGui::Text(m_LanguageManager.Get("settings").c_str());
            ImGui::Separator();

            ImVec2 size(300, 300);

            static int selectedSetting = 0;

            static std::vector<std::string> settings = {
                "general",
                "theme",
                "language",
                "shortcut"
            };

            ImGui::BeginChild("LeftPane", ImVec2(150, size.y), true);
            for (int i = 0; i < settings.size(); i++)
            {
				if (ImGui::Selectable(m_LanguageManager.Get(settings[i]).c_str(), selectedSetting == i))
				{
					selectedSetting = i;
				}                 
            }

            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("RightPanel", ImVec2(320, size.y), true);

            switch (selectedSetting)
            {
            case 0: ImGui::Text(m_LanguageManager.Get("general_settings").c_str());
                ImGui::Separator();

                // Add font size dropdown

                ImGui::Text(m_LanguageManager.Get("font_size").c_str());
                ImGui::SameLine();

                // Create the preview string for the combo box
                char previewLabel[16];
                snprintf(previewLabel, sizeof(previewLabel), "%d", fontSize);

                if (ImGui::BeginCombo("##FontSize", previewLabel))
                {
                    for (int i = minimumFontSize; i <= maxFontSize; i++)
                    {
                        char label[16];
                        snprintf(label, sizeof(label), "%d", i);

                        bool isSelected = (fontSize == i);
                        if (ImGui::Selectable(label, isSelected))
                        {
                            fontSize= i;
							m_pendingFontSizeChange = true;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            case 1: ImGui::Text(m_LanguageManager.Get("theme_settings").c_str()); break;
            case 2: ImGui::Text(m_LanguageManager.Get("language_settings").c_str());
                ImGui::Separator();

                ImGui::Text(m_LanguageManager.Get("language").c_str());
                ImGui::SameLine();

                // Language Names
                currentLangLabel = (m_Language == "tr") ?
                    m_LanguageManager.Get("turkish") : m_LanguageManager.Get("english");

                if (ImGui::BeginCombo("##Language", currentLangLabel.c_str()))
                {
                    const std::vector<std::pair<std::string, std::string>> languages = {
                        {"en", m_LanguageManager.Get("english")},
                        {"tr", m_LanguageManager.Get("turkish")}
                    };

                    for (const auto& lang : languages)
                    {
                        bool isSelected = (m_Language == lang.first);
                        if (ImGui::Selectable(lang.second.c_str(), isSelected))
                        {
                            m_Language = lang.first;
                            SetLanguage(m_Language);
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            break;
            case 3: ImGui::Text(m_LanguageManager.Get("shortcut_settings").c_str()); break;
            }

            ImGui::EndChild();

            if (ImGui::Button(m_LanguageManager.Get("close").c_str()))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
}

void UI::ShowColorPicker() {
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 windowSize = ImVec2(340, 305);
	const ImVec2 desiredPos = ImVec2(0.0f, io.DisplaySize.y - windowSize.y);
	ImGui::SetNextWindowPos(desiredPos, ImGuiCond_FirstUseEver);	
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // Corner roundness
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);        // Border thickness
	//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(120, 120, 120, 100));  // Border color

	//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255, 0, 0, 255));      // Input boxes background
	//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 255, 0, 255));       // Button color
	//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 255, 0, 255));       // Title hover colors
	//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255, 255, 0, 255));    // Border
	//ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 255, 0, 255));    // Border
	//ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 255, 255));    // Border

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
	//ImGui::PopStyleColor(7);  // Border
	//ImGui::PopStyleVar(2);   // Rounding ve BorderSize
}

void UI::ShowToolPanel() {
    float menuBarHeight = ImGui::GetFrameHeight();
	ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight), ImGuiCond_Always);
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
    
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // Corner roundness
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);        // Border thickness
    //ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(120, 120, 120, 100));  // Border color
    //ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255, 0, 0, 255));     // Input box background
    //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 255));      // Button color
    //ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 255, 0, 255));      // Header hover colors
    //ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255, 255, 0, 255));    // Border
    //ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 255, 0, 255));     // Title background
    //ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 255, 255)); // Active title background
    
	

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
    //ImGui::PopStyleColor(7);  // 7 colors were pushed
    //ImGui::PopStyleVar(2);    // 2 style variables were pushed
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
	// If ImGui is active on an item or hovering over a window, no drawing should be done
	return !(ImGui::GetIO().WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));
}

int UI::GetFontSize()
{
	return fontSize;
}

void UI::SetFontSize(int size) {
    fontSize = size;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig font_cfg;
    font_cfg.OversampleH = 2;
    font_cfg.OversampleV = 2;

    const ImWchar turkish_chars[] = {
        0x20, 0x7F, 0x011E, 0x011F, 0x0130, 0x0131,
        0x015E, 0x015F, 0x00C7, 0x00E7,
        0x00D6, 0x00F6, 0x00DC, 0x00FC, 0x00B0, 0x00B0, 0, 0
    };

    const ImWchar* glyph_ranges = m_LanguageManager.GetSystemLanguage() == "tr"
        ? turkish_chars
        : io.Fonts->GetGlyphRangesDefault();

    io.Fonts->AddFontFromFileTTF("Source\\Assets\\Poppins-Regular.ttf", (float)fontSize, &font_cfg, glyph_ranges);
    ImGui::SFML::UpdateFontTexture();
}


std::string UI::GetLanguage()
{
	return m_Language;
}

void UI::SetLanguage(const std::string& language)
{
	m_Language = language;
    m_LanguageManager.Load("Source/Assets/lang_" + m_Language + ".json");
}

void UI::SetExport(bool value)
{
	exportFile = value;
}

bool UI::GetExport() const
{
	return exportFile;
}