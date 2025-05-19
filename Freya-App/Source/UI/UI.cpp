#include "UI.h"
#include <iostream>

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

void UI::SetCanvas(Canvas* canvas) {
	m_Canvas = canvas;
	m_CanvasWidth = m_Canvas->GetSize().x;
	m_CanvasHeight = m_Canvas->GetSize().y;
    m_ProjectManager = std::make_unique<ProjectManager>(*canvas);
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

    if (CanDraw()) 
    {
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

            if (keyPressed->scancode == sf::Keyboard::Scancode::Z && keyPressed->control)
            {
                if (m_Canvas->CanUndo())
                {
                    m_Canvas->Undo();
                }
            }

            if (keyPressed->scancode == sf::Keyboard::Scancode::S && keyPressed->control)
            {
                if (m_LastProjectPath.empty() || !std::filesystem::exists(m_LastProjectPath)) {
                    openSaveProjectDialog = true;
                    m_ProjectDirectory = ProjectManager::GetDefaultProjectDirectory().string();
                    m_ProjectFileName = ProjectManager::GenerateUniqueProjectName(m_ProjectDirectory, "MyProject");
                }
                else {
                    if (!m_ProjectManager->SaveProject(m_LastProjectPath))
                        ShowNotification(m_LanguageManager.Get("cant_save_project").c_str(), true);
                    else
                        ShowNotification(m_LanguageManager.Get("saved_project").c_str());
                }
            }
	    }
    }
}

void UI::Render(sf::RenderWindow& window) {
	ShowMainMenuBar();
	ShowToolPanel();
	ShowColorPicker();
	ShowToolBar();
    RenderNotifications();
	ImGui::SFML::Render(window);
}

void UI::ShowMainMenuBar() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(m_LanguageManager.Get("file").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("new").c_str())) { /* ... */ }
            if (ImGui::MenuItem(m_LanguageManager.Get("save_project").c_str()))
            {
                if (m_LastProjectPath.empty() || !std::filesystem::exists(m_LastProjectPath)) {
                    openSaveProjectDialog = true;
                    m_ProjectDirectory = ProjectManager::GetDefaultProjectDirectory().string();
                    m_ProjectFileName = ProjectManager::GenerateUniqueProjectName(m_ProjectDirectory, "MyProject");
                }
                else {
                    if (!m_ProjectManager->SaveProject(m_LastProjectPath))
                        ShowNotification(m_LanguageManager.Get("cant_save_project").c_str(), true);
                    else
                        ShowNotification(m_LanguageManager.Get("saved_project").c_str());
                }
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("open_project").c_str()))
            {
                std::string path = ShowOpenFreyaDialog();
                if (!path.empty()) {
                    if (!m_ProjectManager->LoadProject(path))
                        ShowNotification(m_LanguageManager.Get("project_load_fail").c_str(), true);
                    else
                    {
                        ShowNotification(m_LanguageManager.Get("project_load_success").c_str());
                        m_LastProjectPath = path;
                        m_OpenedProjectName = std::filesystem::path(path).filename().string();
                    }
                }
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("save_as").c_str())) {
                openSaveProjectDialog = true;
                m_ProjectDirectory = ProjectManager::GetDefaultProjectDirectory().string();
                m_ProjectFileName = ProjectManager::GenerateUniqueProjectName(m_ProjectDirectory, "MyProject");
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("import").c_str()))
            {
                std::string selectedPath = ShowImageDialog();
                if (!selectedPath.empty())
                {
                    sf::Image image;
                    if (image.loadFromFile(selectedPath))
                    {
                        // Save current state before loading new image
                        m_Canvas->SaveState();

                        // Set transparency mode if the image has transparency
                        bool hasTransparency = false;

                        // Check if image has any transparent pixels
                        sf::Vector2u size = image.getSize();
                        for (unsigned int x = 0; x < size.x && !hasTransparency; ++x) {
                            for (unsigned int y = 0; y < size.y && !hasTransparency; ++y) {
                                if (image.getPixel({ x, y }).a < 255) {
                                    hasTransparency = true;
                                }
                            }
                        }

                        // If image has transparency, ensure canvas is in transparent mode
                        if (hasTransparency) {
                            m_Canvas->SetTransparent(true);
                        }

                        // Resize canvas to match the image dimensions
                        m_Canvas->SetSize(image.getSize().x, image.getSize().y);

                        // Convert Image to Texture
                        sf::Texture texture;
                        texture.loadFromImage(image);

                        // Clear canvas with transparent color if needed
                        if (hasTransparency) {
                            m_Canvas->Clear(sf::Color(0, 0, 0, 0));
                        }
                        else {
                            m_Canvas->Clear(sf::Color::White);
                        }

                        // Draw the loaded texture to the canvas's render texture
                        sf::Sprite tempSprite(texture);
                        m_Canvas->GetRenderTexture().draw(tempSprite);
                        m_Canvas->Display(); // Make sure to display the render texture
                    }
                    else
                    {
                        // Handle loading error
                        std::cerr << "Failed to load image: " << selectedPath << std::endl;
                    }
                }
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("export").c_str())) 
            {
                openExportDialog = true;

            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(m_LanguageManager.Get("edit").c_str()))
        {
            if (ImGui::MenuItem(m_LanguageManager.Get("undo").c_str()))
            {
                if (m_Canvas->CanUndo())
                {
                    m_Canvas->Undo();
                }
            }
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
            if (ImGui::MenuItem(m_LanguageManager.Get("resize").c_str())) 
            {
				openResizeDialog = true;
                m_ResizeWidth = m_CanvasWidth;
                m_ResizeHeight = m_CanvasHeight;
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("horizontal_turn").c_str())) 
            {
				m_Canvas->FlipHorizontal();
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("vertical_turn").c_str())) 
            { 
                m_Canvas->FlipVertical();
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("90_clock_rotate").c_str())) 
            {
				m_Canvas->Rotate90();

            }
            if (ImGui::MenuItem(m_LanguageManager.Get("90_counterclock_rotate").c_str())) 
            { 
				m_Canvas->RotateCounterClockwise90();
            }
            if (ImGui::MenuItem(m_LanguageManager.Get("180_rotate").c_str())) 
            { 
				m_Canvas->Rotate180();
            }

            ImGui::EndMenu();
        }

        if (openSaveProjectDialog) {
            ImGui::OpenPopup("SaveProjectPopup");
            openSaveProjectDialog = false;
        }

        if (ImGui::BeginPopup("SaveProjectPopup")) {
            ImGui::Text(m_LanguageManager.Get("save_project").c_str());
            ImGui::Separator();

            ImVec2 size(25 * fontSize, 10 * fontSize);
            ImGui::BeginChild("SaveProjectPanel", size, true);

            const float labelWidth = fontSize * 5;
            const float inputWidth = 250.f;

            ImGui::Text(m_LanguageManager.Get("file_name").c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            static char nameBuffer[256];
            strncpy(nameBuffer, m_ProjectFileName.c_str(), sizeof(nameBuffer));
            if (ImGui::InputText("##ProjectName", nameBuffer, sizeof(nameBuffer))) {
                m_ProjectFileName = nameBuffer;
            }

            ImGui::Text(m_LanguageManager.Get("file_path").c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            static char dirBuffer[512];
            strncpy(dirBuffer, m_ProjectDirectory.c_str(), sizeof(dirBuffer));
            if (ImGui::InputText("##ProjectDir", dirBuffer, sizeof(dirBuffer))) {
                m_ProjectDirectory = dirBuffer;
            }

            ImGui::SameLine();
            if (ImGui::Button(m_LanguageManager.Get("browse").c_str())) {
                std::string path = ShowFileDialog();
                if (!path.empty())
                    m_ProjectDirectory = path;
            }

            ImGui::EndChild();
            ImGui::Separator();

            // Save ve Cancel
            if (ImGui::Button(m_LanguageManager.Get("cancel").c_str(), ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(m_LanguageManager.Get("save").c_str(), ImVec2(120, 0))) {
                std::string finalPath;

                if (m_ProjectDirectory.empty())
                    finalPath = "./";
                else {
                    char lastChar = m_ProjectDirectory.back();
                    if (lastChar != '/' && lastChar != '\\')
                        m_ProjectDirectory += "/";
                    finalPath = m_ProjectDirectory;
                }

                finalPath += m_ProjectFileName;
                if (!finalPath.ends_with(".freya"))
                    finalPath += ".freya";

                if (!m_ProjectManager->SaveProject(finalPath)) {
                    ShowNotification(m_LanguageManager.Get("cant_save_project").c_str(), true);
                }
                else {
                    ShowNotification(m_LanguageManager.Get("saved_project").c_str());
                    m_LastProjectPath = finalPath;
                    m_OpenedProjectName = std::filesystem::path(finalPath).filename().string();
                }

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


        if (openResizeDialog) {
            ImGui::OpenPopup("ResizePopup");
            openResizeDialog = false;
        }

        if (ImGui::BeginPopup("ResizePopup"))
        {
            ImGui::Text(m_LanguageManager.Get("resize").c_str());
            ImGui::Separator();
            ImVec2 size(400, 6 * fontSize);
            ImGui::BeginChild("ResizePanel", size, true);

            // Calculate positions for aligned layout
            const float labelWidth = 120.0f;
            const float inputWidth = 150.0f;

            // Width

            ImGui::Text(m_LanguageManager.Get("width").c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputInt("##Width", &m_ResizeWidth, 1, 10);

            // Height
            ImGui::Text(m_LanguageManager.Get("height").c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputInt("##Height", &m_ResizeHeight, 1, 10);

            ImGui::EndChild(); // End ResizePanel
            ImGui::Separator();

            // Resize & Cancel Buttons
            if (ImGui::Button(m_LanguageManager.Get("cancel").c_str(), ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(m_LanguageManager.Get("resize_button").c_str(), ImVec2(120, 0)))
            {
                // Implement ResizeCanvas function that applies the new dimensions
                ResizeCanvas(m_ResizeWidth, m_ResizeHeight);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
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

        // Export Popup
        if (openExportDialog) {
            ImGui::OpenPopup("ExportPopup");
            openExportDialog = false;
            // Default filename with current date/time
            time_t now = time(0);
            struct tm* timeinfo = localtime(&now);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "image_%Y%m%d_%H%M%S", timeinfo);
            m_ExportFileName = buffer;
        }
        if (ImGui::BeginPopup("ExportPopup"))
        {
            ImGui::Text(m_LanguageManager.Get("export").c_str());
            ImGui::Separator();
            ImVec2 size(25 * fontSize, 10 * fontSize);
            ImGui::BeginChild("ExportPanel", size, true);

            // Calculate positions for aligned layout
            const float labelWidth = fontSize * 5;
			const float inputWidth = 250.0f; // Fixed width for inputs

            // File Name
            ImGui::Text(m_LanguageManager.Get("file_name").c_str());
            ImGui::SameLine(labelWidth); // Fixed position for inputs
            ImGui::SetNextItemWidth(inputWidth);
            char fileNameBuffer[256];
            strncpy(fileNameBuffer, m_ExportFileName.c_str(), sizeof(fileNameBuffer));
            fileNameBuffer[sizeof(fileNameBuffer) - 1] = 0;
            if (ImGui::InputText("##FileName", fileNameBuffer, sizeof(fileNameBuffer)))
            {
                m_ExportFileName = fileNameBuffer;
            }

            // File Path
            ImGui::Text(m_LanguageManager.Get("file_path").c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            char filePathBuffer[256];
            strncpy(filePathBuffer, m_ExportFilePath.c_str(), sizeof(filePathBuffer));
            filePathBuffer[sizeof(filePathBuffer) - 1] = 0;
            if (ImGui::InputText("##FilePath", filePathBuffer, sizeof(filePathBuffer)))
            {
                m_ExportFilePath = filePathBuffer;
            }

            // Browse button on the same line as the path input but after it
            ImGui::SameLine();
            if (ImGui::Button(m_LanguageManager.Get("browse").c_str()))
            {
                std::string selectedPath = ShowFileDialog();
                if (!selectedPath.empty())
                {
                    m_ExportFilePath = selectedPath;
                }
            }

            // Format Selection
            ImGui::Text(m_LanguageManager.Get("format").c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            const char* formats[] = { "PNG", "JPG", "BMP" };
            const char* currentFormat = formats[m_SelectedFormat];
            if (ImGui::BeginCombo("##Format", currentFormat))
            {
                for (int i = 0; i < IM_ARRAYSIZE(formats); i++)
                {
                    bool isSelected = (m_SelectedFormat == i);
                    if (ImGui::Selectable(formats[i], isSelected))
                    {
                        m_SelectedFormat = i;
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::EndChild(); // End ExportPanel
            ImGui::Separator();

            // Export & Cancel Buttons
            if (ImGui::Button(m_LanguageManager.Get("cancel").c_str(), ImVec2(150, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button(m_LanguageManager.Get("export").c_str(), ImVec2(150, 0)))
            {
                ExportImage();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }


        // Settings Popup
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

            ImGui::BeginChild("LeftPanel", ImVec2(150, size.y), true);
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

    //const ImVec2 minSize = ImVec2(300, 250); // Minimum size for the toolbar
    //const ImVec2 maxSize = ImVec2(600, 550); // Maximum size for the toolbar
    const ImVec2 defaultSize = ImVec2(340, 305); // Default window size
    const ImVec2 desiredPos = ImVec2(0.0f, io.DisplaySize.y - defaultSize.y);

	ImGui::SetNextWindowPos(desiredPos, ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // Corner roundness
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);        // Border thickness
	//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(120, 120, 120, 100));  // Border color

	//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255, 0, 0, 255));      // Input boxes background
	//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 255, 0, 255));       // Button color
	//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 255, 0, 255));       // Title hover colors
	//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255, 255, 0, 255));    // Border
	//ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 255, 0, 255));    // Border
	//ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 255, 255));    // Border

    ImGui::Begin("color", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(m_LanguageManager.Get("color_picker_title").c_str());
	ImGui::Separator();

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
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

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
    //const ImVec2 minSize = ImVec2(fontSize * 4.5f, 300); // Minimum size for the toolbar
    //const ImVec2 maxSize = ImVec2(200, 600); // Maximum size for the toolbar
    const ImVec2 defaultSize = ImVec2(fontSize * 4.5f, 300); // Default window size

    const float snapThreshold = 20.0f; // Snap threshold in pixels
    
    // Position on the right side of the screen
    const ImVec2 desiredPos = ImVec2(io.DisplaySize.x - defaultSize.x, 
                                    (io.DisplaySize.y - defaultSize.y) / 2);
    ImGui::SetNextWindowPos(desiredPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
    
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
    ImGui::Begin("tool", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::Text(m_LanguageManager.Get("tool_title").c_str());
	ImGui::Separator();
    
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

void UI::ExportImage()
{
    if (!m_Canvas)
        return;

    // Prepare the file extension based on the selected format
    std::string fileExtension;
    switch (m_SelectedFormat)
    {
    case 0: fileExtension = ".png"; break;
    case 1: fileExtension = ".jpg"; break;
    case 2: fileExtension = ".bmp"; break;
    default: fileExtension = ".png"; break;
    }

    // Ensure filename has proper extension
    std::string fileName = m_ExportFileName;
    std::string extension = fileName.substr(fileName.find_last_of(".") + 1);

    // If no extension or different extension, add the correct one
    if (fileName.find(".") == std::string::npos ||
        (extension != "png" && extension != "jpg" && extension != "bmp"))
    {
        fileName += fileExtension;
    }

    // Build full path
    std::string fullPath;
    if (m_ExportFilePath.empty())
    {
        fullPath = fileName;
    }
    else
    {
        // Ensure path has trailing slash
        if (m_ExportFilePath.back() != '/' && m_ExportFilePath.back() != '\\')
        {
            m_ExportFilePath += '/';
        }
        fullPath = m_ExportFilePath + fileName;
    }

    // Export based on format
    bool success = false;
    switch (m_SelectedFormat)
    {
    case 0: // PNG
        success = m_Canvas->ExportToPNG(fullPath);
        break;
    case 1: // JPG
        success = m_Canvas->ExportToJPG(fullPath);
        break;
    case 2: // BMP
        success = m_Canvas->ExportToBMP(fullPath);
        break;
    }

    if (success) {
        // Set a notification flag or add to a notification queue
        ShowNotification(m_LanguageManager.Get("export_success"));
    }
    else {
        ShowNotification(m_LanguageManager.Get("export_failure"), true);
    }
}

void UI::ShowNotification(const std::string& message, bool isError) {
    Notification notification;
    notification.message = message;
    notification.timeRemaining = 3.0f; // Show for 3 seconds
    notification.isError = isError;
    m_Notifications.push_back(notification);
}

void UI::RenderNotifications() {
    const float NOTIFICATION_WIDTH = 200.0f;
    const float NOTIFICATION_HEIGHT = 50.0f;
    const float NOTIFICATION_PADDING = 10.0f;

    ImGuiIO& io = ImGui::GetIO();
    float deltaTime = io.DeltaTime;

    // Update notification times and remove expired ones
    for (auto it = m_Notifications.begin(); it != m_Notifications.end();) {
        it->timeRemaining -= deltaTime;
        if (it->timeRemaining <= 0) {
            it = m_Notifications.erase(it);
        }
        else {
            ++it;
        }
    }

    // Render active notifications
    float yPos = io.DisplaySize.y - NOTIFICATION_HEIGHT * 2; // Start from top

    for (const auto& notification : m_Notifications) {
        ImVec2 pos(io.DisplaySize.x - NOTIFICATION_WIDTH - NOTIFICATION_PADDING, yPos);
        ImVec2 size(NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT);

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        // Set window color based on notification type
        if (notification.isError) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.0f, 0.0f, 0.9f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.5f, 0.0f, 0.9f));
        }

        // Calculate alpha for fade out effect
        float alpha = notification.timeRemaining < 1.0f ? notification.timeRemaining : 1.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        // Begin window with unique ID
        ImGui::Begin(("##Notification" + std::to_string((intptr_t)&notification)).c_str(), nullptr, flags);

        // Center text
        float textWidth = ImGui::CalcTextSize(notification.message.c_str()).x;
        float textX = (NOTIFICATION_WIDTH - textWidth) * 0.5f;
        float textY = (NOTIFICATION_HEIGHT - ImGui::GetTextLineHeight()) * 0.5f;

        ImGui::SetCursorPos(ImVec2(textX, textY));
        ImGui::Text("%s", notification.message.c_str());

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        yPos += NOTIFICATION_HEIGHT + NOTIFICATION_PADDING;
    }
}

void UI::ResizeCanvas(int width, int height)
{
    if (m_Canvas)
    {
        m_Canvas->SaveState();
        if (width < 1 || height < 1 || width > 32768 || height > 32768)
        {
            ShowNotification(m_LanguageManager.Get("invalid_size"), true);
            return;
        }
        m_Canvas->SetSize(width, height);
        m_CanvasWidth = width;
        m_CanvasHeight = height;
    }
}

std::string UI::ShowFileDialog()
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(m_ProjectDirectory.c_str(), &outPath);
	
    std::string resultPath;
    if (result == NFD_OKAY && outPath)
    {
        resultPath = outPath;
        free(outPath);
    }
    else if (result == NFD_CANCEL)
    {

    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
    }

    return resultPath;
}

std::string UI::ShowImageDialog()
{
    nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("png,jpg,bmp", nullptr, &outPath);

    std::string resultPath;
	if (result == NFD_OKAY && outPath)
	{
		resultPath = outPath;
		free(outPath);
	}
	else if (result == NFD_CANCEL)
	{

	}
	else
	{
		printf("Error: %s\n", NFD_GetError());
	}
    return resultPath;
}

std::string UI::ShowOpenFreyaDialog()
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog("freya", nullptr, &outPath);

    std::string resultPath;
    if (result == NFD_OKAY && outPath) {
        resultPath = outPath;
        free(outPath);
    }
    else if (result == NFD_CANCEL) {

    }
    else {
        printf("Error: %s\n", NFD_GetError());
    }

    return resultPath;
}

std::string UI::GetOpenedProjectName() const
{
	return m_OpenedProjectName;
}