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
	0x20, 0x7F,       // Latin temel karakterler
	0x011E, 0x011F,   // �, �
	0x0130, 0x0131,   // �, �
	0x015E, 0x015F,   // �, �
	0x00C7, 0x00E7,   // �, �
	0x00D6, 0x00F6,   // �, �
	0x00DC, 0x00FC,   // �, �
	0, 0              // end
	};

	ImFontConfig font_cfg;
	font_cfg.OversampleH = 2;
	font_cfg.OversampleV = 2;

	fontSize = 18.f;

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
}

void UI::Render(sf::RenderWindow& window) {
	//ShowMainMenuBar();
	ShowToolPanel();
	ShowStatusBar();
	ShowColorPicker();
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
	const float snapThreshold = 20.0f; // Snap when within 20 pixels of the edge
    const ImVec2 windowSize = ImVec2(340, 305);
    const ImVec2 padding = ImVec2(10, 10);
	const ImVec2 desiredPos = ImVec2(0.0f, io.DisplaySize.y - windowSize.y);
	ImGui::SetNextWindowPos(desiredPos, ImGuiCond_FirstUseEver);	
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // K��e yuvarlakl���
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);        // Kenarl�k kal�nl���
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(120, 120, 120, 100));  // Kenarl�k rengi

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255, 0, 0, 255));      // Giri� kutular� arka plan�
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 255, 0, 255));       // D��me rengi
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 255, 0, 255));       // Ba�l�k hover renkleri
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255, 255, 0, 255));    // Kenarl�k
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 255, 0, 255));    // Kenarl�k
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 255, 255));    // Kenarl�k

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

	// Opacity (0.0 - 1.0)
	ImGui::Text(m_LanguageManager.Get("opacity").c_str());
	ImGui::SetCursorPos(ImVec2(pos2.x, pos2.y + fontSize));
	ImGui::SliderFloat("##Opacity", &m_Color[3], 0.0f, 1.f);

	ImGui::SetCursorPos(ImVec2(currentCurPos.x + 340, pos2.y));

	// Spacing (0.0 - 1.0)
	ImGui::Text(m_LanguageManager.Get("spacing").c_str());
	ImGui::SetCursorPos(ImVec2(currentCurPos.x + 340, pos2.y + fontSize));
	ImGui::SliderInt("##Spacing", &m_Spacing, 0, 500);

	// Color
	ImGui::SetCursorPos(ImVec2(currentCurPos.x + 510, pos2.y));

	ImGui::Text(m_LanguageManager.Get("color").c_str());
	ImGui::SetCursorPos(ImVec2(currentCurPos.x + 510, pos2.y + fontSize));
	ImGui::ColorEdit4("##Color", m_Color, ImGuiColorEditFlags_NoInputs);

	ImGui::PopItemWidth();

	ImGui::End();
}



sf::Color UI::GetColor() 
{
   return ConvertToSFMLColor();
}

float UI::GetBrushSize() {
	return (float)m_BrushSize;
}

float UI::GetSpacing() {
	return m_Spacing;
}
sf::Color UI::ConvertToSFMLColor() {
	return sf::Color(
		static_cast<std::uint8_t>(m_Color[0] * 255),
		static_cast<std::uint8_t>(m_Color[1] * 255),
		static_cast<std::uint8_t>(m_Color[2] * 255),
		static_cast<std::uint8_t>(m_Color[3] * 255)
	);
}

//void UI::ShowToolPanel() {
//    ImGuiIO& io = ImGui::GetIO();
//    ImVec2 windowSize = io.DisplaySize;
//    
//    // Anchor tipi de�i�ti�inde varsay�lan boyutlar
//    const ImVec2 HORIZONTAL_SIZE = ImVec2(500.f, 100.f); // �st/alt panel boyutu
//    const ImVec2 VERTICAL_SIZE = ImVec2(100.f, 500.f);   // Sa�/sol panel boyutu
//    
//    // �lk ba�latma
//    if (!m_isPanelSizeInitialized) {
//        // Set position to right anchor on start 
//        switch (m_toolBarCurrentAnchor) {
//        case AnchorType::Left:
//            m_toolPanelSize = VERTICAL_SIZE;
//            m_toolPanelPos = ImVec2(0, windowSize.y / 2 - m_toolPanelSize.y / 2);
//            break;
//        case AnchorType::Right:
//            m_toolPanelSize = VERTICAL_SIZE;
//            m_toolPanelPos = ImVec2(windowSize.x - m_toolPanelSize.x, windowSize.y / 2 - m_toolPanelSize.y / 2);
//            break;
//        case AnchorType::Top:
//            m_toolPanelSize = HORIZONTAL_SIZE;
//            m_toolPanelPos = ImVec2(windowSize.x / 2 - m_toolPanelSize.x / 2, 0);
//            break;
//        case AnchorType::Bottom:
//            m_toolPanelSize = HORIZONTAL_SIZE;
//            m_toolPanelPos = ImVec2(windowSize.x / 2 - m_toolPanelSize.x / 2, windowSize.y - m_toolPanelSize.y);
//            break;
//        default:
//            m_toolPanelPos = ImVec2(windowSize.x / 2 - m_toolPanelSize.x / 2, windowSize.y / 2 - m_toolPanelSize.y / 2);
//            break;
//        }
//        m_isPanelSizeInitialized = true;
//    }
//    
//    static bool positionSetThisFrame = false;
//    static bool sizeSetThisFrame = false;
//    static ImVec2 lastWindowSize = windowSize;
//    static AnchorType prevAnchor = m_toolBarCurrentAnchor;
//    
//    // Anchor tipi de�i�ti�inde boyut ve pozisyon g�ncelleme
//    if (prevAnchor != m_toolBarCurrentAnchor) {
//        // Anchor pozisyon de�i�ikli�i durumunda boyut ve pozisyonu birlikte g�ncelle
//        switch (m_toolBarCurrentAnchor) {
//            case AnchorType::Left:
//                m_toolPanelSize = VERTICAL_SIZE;
//                m_toolPanelPos = ImVec2(0, windowSize.y / 2 - m_toolPanelSize.y / 2);
//                break;
//            case AnchorType::Right:
//                m_toolPanelSize = VERTICAL_SIZE;
//                m_toolPanelPos = ImVec2(windowSize.x - m_toolPanelSize.x, windowSize.y / 2 - m_toolPanelSize.y / 2);
//                break;
//            case AnchorType::Top:
//                m_toolPanelSize = HORIZONTAL_SIZE;
//                m_toolPanelPos = ImVec2(windowSize.x / 2 - m_toolPanelSize.x / 2, 0);
//                break;
//            case AnchorType::Bottom:
//                m_toolPanelSize = HORIZONTAL_SIZE;
//                m_toolPanelPos = ImVec2(windowSize.x / 2 - m_toolPanelSize.x / 2, windowSize.y - m_toolPanelSize.y);
//                break;
//            default:
//                // None anchor durumunda pozisyonu koruyoruz, boyutu de�i�tirmiyoruz
//                break;
//        }
//        
//        sizeSetThisFrame = true;
//        positionSetThisFrame = true;
//        prevAnchor = m_toolBarCurrentAnchor;
//    }
//    
//    // Ekran boyutu de�i�ti�inde anchor pozisyonunu g�ncelle
//    if (lastWindowSize.x != windowSize.x || lastWindowSize.y != windowSize.y) {
//        positionSetThisFrame = true;
//        
//        // Ekran boyutu de�i�ti�inde anchor pozisyonunu g�ncelle
//        switch (m_toolBarCurrentAnchor) {
//            case AnchorType::Left:
//                m_toolPanelPos.x = 0;
//                m_toolPanelPos.y = windowSize.y / 2 - m_toolPanelSize.y / 2;
//                break;
//            case AnchorType::Right:
//                m_toolPanelPos.x = windowSize.x - m_toolPanelSize.x;
//                m_toolPanelPos.y = windowSize.y / 2 - m_toolPanelSize.y / 2;
//                break;
//            case AnchorType::Top:
//                m_toolPanelPos.x = windowSize.x / 2 - m_toolPanelSize.x / 2;
//                m_toolPanelPos.y = 0;
//                break;
//            case AnchorType::Bottom:
//                m_toolPanelPos.x = windowSize.x / 2 - m_toolPanelSize.x / 2;
//                m_toolPanelPos.y = windowSize.y - m_toolPanelSize.y;
//                break;
//        }
//        lastWindowSize = windowSize;
//    }
//    
//    // Gerekti�inde pozisyon ve boyutu g�ncelle
//    if (positionSetThisFrame) {
//        ImGui::SetNextWindowPos(m_toolPanelPos);
//        positionSetThisFrame = false;
//    }
//    
//    if (sizeSetThisFrame) {
//        ImGui::SetNextWindowSize(m_toolPanelSize);
//        sizeSetThisFrame = false;
//    } else {
//        ImGui::SetNextWindowSize(m_toolPanelSize, ImGuiCond_Once);
//    }
//    
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
//    
//    ImGuiWindowFlags flags = 0;
//    flags |= ImGuiWindowFlags_NoCollapse, ImGuiWindowFlags_NoSavedSettings;
//
//    
//    // Panel g�r�n�m�n� olu�tur
//    ImGui::Begin("Tools", nullptr, flags);
//    
//    // Ara� butonlar�
//    if (ImGui::Button("Tool 1", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { /* ... */ }
//    if (ImGui::Button("Tool 2", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { /* ... */ }
//    
//    // S�r�kleme takibi
//    if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(0)) {
//        m_isDragging = true;
//        // S�r�kleme ba�lad���nda anchor'u kald�r
//        if (m_toolBarCurrentAnchor != AnchorType::None) {
//            m_toolBarCurrentAnchor = AnchorType::None;
//            prevAnchor = AnchorType::None; // G�ncelleme k�sm�n� da s�f�rla
//        }
//    }
//    
//    // Pozisyon ve boyut de�i�ikliklerini takip et
//    ImVec2 newPos = ImGui::GetWindowPos();
//    ImVec2 newSize = ImGui::GetWindowSize();
//    
//    // De�i�iklikleri kaydet
//    if (newPos.x != m_toolPanelPos.x || newPos.y != m_toolPanelPos.y) {
//        m_toolPanelPos = newPos;
//    }
//    
//    if (newSize.x != m_toolPanelSize.x || newSize.y != m_toolPanelSize.y) {
//        m_toolPanelSize = newSize;
//    }
//    
//    // S�r�kleme bitti�inde anchor kontrol�
//    if (m_isDragging && !ImGui::IsMouseDragging(0)) {
//        m_isDragging = false;
//        
//        // Anchor kontrol�
//        float leftDist = m_toolPanelPos.x;
//        float rightDist = windowSize.x - (m_toolPanelPos.x + m_toolPanelSize.x);
//        float topDist = m_toolPanelPos.y;
//        float bottomDist = windowSize.y - (m_toolPanelPos.y + m_toolPanelSize.y);
//        
//        // �nceki anchoru temizle
//        m_toolBarCurrentAnchor = AnchorType::None;
//        
//        // En yak�n kenara yap��
//        float minDist = SNAP_DISTANCE;
//        if (leftDist <= minDist && leftDist <= rightDist && leftDist <= topDist && leftDist <= bottomDist) {
//            m_toolBarCurrentAnchor = AnchorType::Left;
//            // Boyut ve pozisyon de�i�imi i�in prevAnchor'u g�ncelleme
//            // B�ylece bir sonraki frame'de boyut ve pozisyon otomatik g�ncellenecek
//            if (prevAnchor != AnchorType::Left) {
//                prevAnchor = AnchorType::None; // Tetikleyici g�ncelleme
//            }
//        }
//        else if (rightDist <= minDist && rightDist <= leftDist && rightDist <= topDist && rightDist <= bottomDist) {
//            m_toolBarCurrentAnchor = AnchorType::Right;
//            if (prevAnchor != AnchorType::Right) {
//                prevAnchor = AnchorType::None; // Tetikleyici g�ncelleme
//            }
//        }
//        else if (topDist <= minDist && topDist <= leftDist && topDist <= rightDist && topDist <= bottomDist) {
//            m_toolBarCurrentAnchor = AnchorType::Top;
//            if (prevAnchor != AnchorType::Top) {
//                prevAnchor = AnchorType::None; // Tetikleyici g�ncelleme
//            }
//        }
//        else if (bottomDist <= minDist && bottomDist <= leftDist && bottomDist <= rightDist && bottomDist <= topDist) {
//            m_toolBarCurrentAnchor = AnchorType::Bottom;
//            if (prevAnchor != AnchorType::Bottom) {
//                prevAnchor = AnchorType::None; // Tetikleyici g�ncelleme
//            }
//        }
//    }
//    
//    // Debug bilgisi
//    
//    ImGui::Separator();
//    ImGui::Text("Anchor: %d", static_cast<int>(m_toolBarCurrentAnchor));
//    ImGui::Text("Pos: %.1f, %.1f", m_toolPanelPos.x, m_toolPanelPos.y);
//    ImGui::Text("Size: %.1f, %.1f", m_toolPanelSize.x, m_toolPanelSize.y);
//    ImGui::Text("Dragging: %s", m_isDragging ? "Yes" : "No");
//    
//    
//    ImGui::End();
//    ImGui::PopStyleVar();
//}